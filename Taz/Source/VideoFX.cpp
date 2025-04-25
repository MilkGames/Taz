// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : VideoFX.cpp
//   Purpose : Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"			// PP: global stuff
#include "actors.h"			// PP: actor stuff			
#include "control.h"		// PP: control stuff
#include "main.h"			// PP: main stuff
#include "Colour.h"			// PP: handy colour struct
#include "Glare.h"			// PP: Light glare effects
#include "display.h"		// PP: display stuff
#include "Rectangle.h"		// PP: handy rectangle class
#include "Cheats.h"			// PP: Cheats
#include "util.h"			// PP: Utility functions
#include "shadow.h"			// PP: shadow stuff
#include "lights.h"			// PP: light stuff
#include "LoopArray.h"		// PP: Looping array class - please use it!
#include "cameracase.h"		// PP: camera case stuff

#include "attach.h"			// PP: attached object stuff
#include "health.h"			// PP: 'ealf
#include "characters.h"		// PP: provides character response code

#include "VideoFX.h"		// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!


// PP: bit combination of all video effect currently active
static int				VFX_effectsActive=VFX_NONE;

// PP: index of effect with exclusive use of the VFX_renderTarget, or VFX_NONE if it's free
EVideoEffect			VFX_renderTargetUser=VFX_NONE;

// PP: are we using grey stereoscopy, or colour?
static bool				VFX_stereoGrey;

// PP: 0.0f(none)..1.0f(all) current motion blur level for this frame
static float			VFX_motionBlurLevel;
static unsigned char	VFX_motionBlurAlpha=128;// PP: corresponding motion blur alpha

// PP: VFX_motionBlurSwitchOffPlease indicates that motion blur has been switched off during this frame, so this will be the last frame of post-draw before switching back to the primary render target
static bool				VFX_motionBlurSwitchOffPlease;

// PP: VFX_permanentRenderTarget specifies whether or not the render target should be freed when not in use
static bool				VFX_permanentRenderTarget=false;

// PP: some stuff for VFX_TIMESLICE...
static VEC				VFX_timesliceCamPosCopy;			// PP: copy of camera's position
static CAMERAINFO*		VFX_timesliceCam;					// PP: the camera doing the timeslice

/*
// PP: TODO:

	full-screen/background defocus/blur - similar to what I'm doing on the shadows
	cross-fade - render one frame to the VFX_renderTarget, fade it progressively over the screen as you change scene!
	heat haze  - slap the texture onto a grid of polys and about with the uv's
	depth-of-field - point 2 side-by-side cameras at a focal point
*/



// PP: TEMP!(?) // PP: Aye, temp I'd say
#if (BPLATFORM == PS2)
#define VFX_RENDER_TARGET_WIDTH		512
#define VFX_RENDER_TARGET_HEIGHT	256
#elif (BPLATFORM == XBOX)
#define VFX_RENDER_TARGET_WIDTH		640
#define VFX_RENDER_TARGET_HEIGHT	480//480
#elif (BPLATFORM==GAMECUBE)
#define VFX_RENDER_TARGET_WIDTH		320
#define VFX_RENDER_TARGET_HEIGHT	240 // Nice and small (quarter of the screen)
#else// PP: PC
#define VFX_RENDER_TARGET_WIDTH		512
#define VFX_RENDER_TARGET_HEIGHT	256
#endif

// PP: multi-purpose render target used for video effects
TBRenderTarget*				VFX_renderTarget=NULL;

RENDERTARGETINFO	VFX_renderTargetInfo;


/*	--------------------------------------------------------------------------------
	Function 	: VFX_releaseRenderTarget
	Purpose 	: stop using the VFX_renderTarget for the specified effect
	Parameters 	: effect which will no longer use the render target
	Returns 	: 
	Info 		: releases render target and resets VFX_renderTargetUser
*/

#define TIME_RT_DELETION

static void VFX_releaseRenderTarget(const EVideoEffect effect)
{
	// PP: if this effect is set as the one exclusively using the VFX_renderTarget, unset it as such
	if(VFX_renderTargetUser == effect)
	{
		VFX_renderTargetUser=VFX_NONE;

		if(VFX_renderTarget != NULL)
		{
			if(!VFX_permanentRenderTarget)
			{
#ifdef TIME_RT_DELETION
				TBTimerValue		RTdeleteStart;

				RTdeleteStart=bkTimerRead();
#endif// PP: TIME_RT_CREATION

				// PP: NOTE: It is essential that the specified render target not be current on any texture stage when deleted, if this occurs a crash is likely.

				bdSetTexture(0, NULL);

				bdDeleteRenderTarget(VFX_renderTarget);

			//	bkPrintf("VFX_releaseRenderTarget: deleted render target\n");

				VFX_renderTarget=NULL;

#ifdef TIME_RT_DELETION
				TBTimerValue		RTdeleteEnd;
				TBTimerValue		RTdeleteDelta;

				RTdeleteEnd=bkTimerRead();

				RTdeleteDelta=bkTimerDelta(RTdeleteStart,RTdeleteEnd);

				bkPrintf("<<<<<<< RT DELETION TOOK %5.2f SCANLINES\n", bkTimerToScanlines(RTdeleteDelta, 60, 480));
#endif// PP: TIME_RT_CREATION

			}
		}
	}

//	bkPrintf("VFX_releaseRenderTarget\n");
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_useRenderTarget
	Purpose 	: use the VFX_renderTarget for the specified effect
	Parameters 	: effect which will use the render target
	Returns 	: 
	Info 		: creates render target and sets VFX_renderTargetUser
*/

// PP: OOD: Specify a lockable render target if we're on PS2.  On PC, well maybe I'll just have to use D3D calls
//#if (BPLATFORM==PS2)
//#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ// PP: BCREATERENDERTARGET_LOCKABLE creates targets that badly leak/fragment memory - should maybe talk to tech team at some point
//#define VFX_RENDER_TARGET_COLOUR_DEPTH		16
//#else // if (BPLATFORM==PS2)
//#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
//#define VFX_RENDER_TARGET_COLOUR_DEPTH		32
//#endif// PP: (BPLATFORM==PS2)

// NH: All platforms seem to handle render targets slightly differently. For more info, best to check tech.net
#if (BPLATFORM==PS2)
#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ// PP: BCREATERENDERTARGET_LOCKABLE creates targets that badly leak/fragment memory - should maybe talk to tech team at some point
#define VFX_RENDER_TARGET_COLOUR_DEPTH		16
#elif (BPLATFORM==PC)
#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
#define VFX_RENDER_TARGET_COLOUR_DEPTH		videoMode.bppScreen
#elif (BPLATFORM==GAMECUBE)
#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
#define VFX_RENDER_TARGET_COLOUR_DEPTH		16
#else
#define VFX_RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
#define VFX_RENDER_TARGET_COLOUR_DEPTH		32
#endif

//#define TIME_RT_CREATION

static void VFX_useRenderTarget(const EVideoEffect effect)
{
	// PP: create the VFX_renderTarget if it doesn't exist...

	if(VFX_renderTarget == NULL)
	{
#ifdef TIME_RT_CREATION
		TBTimerValue		RTcreateStart;

		RTcreateStart=bkTimerRead();
#endif// PP: TIME_RT_CREATION

// PP: NOTE: rgb & z depth ought to match
		if (VFX_renderTarget = bdCreateRenderTarget(VFX_RENDER_TARGET_WIDTH, VFX_RENDER_TARGET_HEIGHT, VFX_RENDER_TARGET_COLOUR_DEPTH, VFX_RENDER_TARGET_COLOUR_DEPTH, VFX_RENDER_TARGET_CREATION_FLAGS))
		{
			// retrieve the dimensions and depths that were actually used
			bdGetRenderTargetInfo(VFX_renderTarget,
									&(VFX_renderTargetInfo.width),
									&(VFX_renderTargetInfo.height),
									&(VFX_renderTargetInfo.rgbDepth),
									&(VFX_renderTargetInfo.zDepth));

#ifdef _TAZDEBUG

			bkPrintf("VFX_useRenderTarget: created render target successfully (%d*%d, %d bit RGB, %d bit Z)\n",
				VFX_renderTargetInfo.width, VFX_renderTargetInfo.height, VFX_renderTargetInfo.rgbDepth, VFX_renderTargetInfo.zDepth);
		}
		else
		{
			bkPrintf("VFX_useRenderTarget: failed to create render target (%d*%d, %d bit RGB, %d bit Z)\n",
				VFX_RENDER_TARGET_WIDTH, VFX_RENDER_TARGET_HEIGHT, VFX_RENDER_TARGET_COLOUR_DEPTH, VFX_RENDER_TARGET_COLOUR_DEPTH);

#endif// PP: def _TAZDEBUG
		}

#ifdef TIME_RT_CREATION
		TBTimerValue		RTcreateEnd;
		TBTimerValue		RTcreateDelta;

		RTcreateEnd=bkTimerRead();

		RTcreateDelta=bkTimerDelta(RTcreateStart,RTcreateEnd);

		bkPrintf(">>>>>>> RT CREATION TOOK %5.2f SCANLINES\n", bkTimerToScanlines(RTcreateDelta, 60, 480));
#endif// PP: TIME_RT_CREATION
	}

	// PP: record this effect as being the one exclusively using the VFX_renderTarget
	VFX_renderTargetUser=effect;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_bookPermanentRenderTarget
	Purpose 	: specify that the VFX_renderTarget should NOT be freed when out of use
	Parameters 	: 
	Returns 	: 
	Info 		: eg. in the West Boss, where the target gets used a lot for motion blur and we want to prevent fragmentation)
					NOTE: this does not actually create the render target
*/
void VFX_bookPermanentRenderTarget(void)
{
	VFX_permanentRenderTarget=true;

	// PP: TEMP BODGE till I set up a VFX_ type for offscreen books
	// PP: This does no harm anyway, just looks untidy
	VFX_useRenderTarget(VFX_NONE);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_permanentRenderTarget
	Purpose 	: release the render target that was booked as a permanent target by VFX_bookPermanentRenderTarget
	Parameters 	: 
	Returns 	: 
	Info 		: doesn't try to release the target if it hasn't been created
*/
void VFX_releasePermanentRenderTarget(void)
{
	// PP: override this function if we have a permanent-permanent render target
	if(!VFX_PERMANENT_RENDER_TARGET)
	{
		VFX_permanentRenderTarget=false;

		if(VFX_renderTarget != NULL)
		{
			// PP: NOTE: It is essential that the specified render target not be current on any texture stage when deleted, if this occurs a crash is likely.

			bdSetTexture(0, NULL);

			bdDeleteRenderTarget(VFX_renderTarget);

			VFX_renderTarget=NULL;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_effectActive
	Purpose 	: determine if a particular effect is active
	Parameters 	: effect to check for
	Returns 	: true if the effect is active, false if not
	Info 		: 
*/
bool VFX_effectActive(const EVideoEffect effect)
{
	return ((VFX_effectsActive & (1<<(effect-1))) != 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_deactivateEffect
	Purpose 	: deactivate an effect, switch it off
	Parameters 	: index of effect to deactivate
	Returns 	: 
	Info 		: *** THIS ASSUMES THAT THE APPROPRIATE CHECKS HAVE ALREADY BEEN DONE TO PROTECT CHEAT EFFECTS ***
*/
void VFX_deactivateEffect(const EVideoEffect effect)
{
	switch(effect)
	{
	case VFX_MOTIONBLUR:
		{
			VFX_motionBlurLevel = 0.0f;
			VFX_motionBlurAlpha = 128;

#if (BPLATFORM == PS2)

			bDisplayInfo.motionBlurVal=VFX_motionBlurAlpha;

#endif // PP: BPLATFORM == PS2

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET

			// PP: clear the screen when page-flipping
			bdSetRenderState(BDRENDERSTATE_CLEAR, BDCLEARMODE_ALL, 0);

			// PP: stop using the VFX_renderTarget for this effect
			VFX_releaseRenderTarget(VFX_MOTIONBLUR);

#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

		}
		break;

	case VFX_SHADOWMAP:
		{
			// PP: reset normal cartoon light vector
			baSetDirectionalLight(&cartoonLight, cartoonLightVec);
			// PP: TEMP BABEL WORKAROUND - fixes light direction problem YAY!!!!
			cartoonLight.directional.unitDirection[W]=1.0f;

			// PP: release the shadow map render target
			VFX_releaseShadowRenderTarget();
		
			// PP: unusual case - gotta AND-out the flag THEN do something else
			VFX_effectsActive &= ~(1<<(effect-1));

			// PP: NEW: MUST RE-PREPARE ALL ACTOR SHADOWS (this is done from PrepareScene)
			PrepareScene(gameStatus.currentScene);

			return;
		}
		break;

	case VFX_TIMESLICE:
		{
			// PP: remove film borders
			setFilmBorders(false);

			// PP: restore camera pos
			SetCameraPosition(VFX_timesliceCam, VFX_timesliceCamPosCopy);

			// PP: restart time
			SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
		}
		break;

	case VFX_SPINOFF:
		{
			// PP: clear some flags...

			VFX_spinoffPrePreparing=false;
			VFX_spinoffPreparing=false;
			VFX_spinoffPrepared=false;
			VFX_spinoffNewlyPrepared=false;
			VFX_spinoffStarted=false;
			VFX_spinoffNewlyStarted=false;
		}
		break;

	case VFX_STEREO:
		{
			// PP: re-enable all framewrite channels
			rgbFramewriteChannels=BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE;
			// PP: REMOUT: OOD		allFramewriteChannels=rgbFramewriteChannels|BDFRAMEWRITE_ALPHA;

			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 0);
			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 1);

			videoMode.drawNum=0;	// PP: just in case

			videoMode.numDraws=1;	// PP: kinda important!
		}
		break;

	case VFX_FOCUS:
		{
	// PP: put Taz's cel shading back on
#ifndef CARTOON_DOF

			// PP: need to do this otherwise EnableActorCartoonRenderMode will just call EnableActorLineRenderMode
			VFX_effectsActive &= ~(1<<(effect-1));

			EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);

#endif // PP: def CARTOON_DOF
		}
		break;

	}

	// PP: stop using the VFX_renderTarget for this effect
	VFX_releaseRenderTarget(effect);

	VFX_effectsActive &= ~(1<<(effect-1));
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_activateEffect
	Purpose 	: activate an effect, switch it on
	Parameters 	: index of effect to activate, (opt/false) priority flag
	Returns 	: true if the activation succeeded, false if not
	Info 		: 
*/
bool VFX_activateEffect(const EVideoEffect effect, const bool priority)
{
	// IH: No blur on cube
#if BPLATFORM==GAMECUBE
	return false;
#endif

	switch(effect)
	{
	case VFX_MOTIONBLUR:
		{

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET

			// PP: use the VFX_renderTarget for this effect
			VFX_useRenderTarget(effect);

			// PP: don't clear the screen when page-flipping
			bdSetRenderState(BDRENDERSTATE_CLEAR, BDCLEARMODE_Z, 0);

#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

		}
		break;

	case VFX_STEREO:
		{
			videoMode.numDraws=2;	// PP: each frame will be drawn in two passes (one for each eye, duh!)
			videoMode.drawNum=0;	// PP: just in case

			if(VFX_stereoGrey)
			{
				// PP: re-enable all framewrite channels
				rgbFramewriteChannels=BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE;
				// PP: REMOUT: OOD		allFramewriteChannels=rgbFramewriteChannels|BDFRAMEWRITE_ALPHA;
			}

			if(VFX_stereoUseRenderTarget)
			{
				// PP: use the VFX_renderTarget for this effect
				VFX_useRenderTarget(effect);
			}
		}
		break;

	case VFX_SHADOWMAP:
		{
			// PP: create the shadow map render target
			VFX_createShadowRenderTarget();

			// PP: unusual case - gotta OR-in the flag THEN do something else
			VFX_effectsActive |= (1<<(effect-1));

			// PP: NEW: MUST RE-PREPARE ALL ACTOR SHADOWS (this is done from PrepareScene)
			PrepareScene(gameStatus.currentScene);

			return true;
		}
		break;

	case VFX_SPINOFF:
		{

		}
		break;

	case VFX_TRANSITION:
		{

		}
		break;

	case VFX_FOCUS:
		{
	// PP: TEMP I HOPE: take off Taz's cel shading if it doesn't work with DOF
#ifndef CARTOON_DOF
			EnableActorLineRenderMode(&gameStatus.player1->actorInstance);
#endif // PP: def CARTOON_DOF
		}
		break;
	}

	VFX_effectsActive |= (1<<(effect-1));

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_init
	Purpose 	: Initialise video effects
	Parameters 	: 
	Returns 	: 
	Fact 		: #1. The theme music to the popular children's television programme 
						"Button Moon" was sung by Peter Davison and Sandra Dickinson.
*/
void VFX_init(void)
{
	// PP: if this platform has enough video memory...
	if(VFX_PERMANENT_RENDER_TARGET)
	{
		// PP: ...book a permanent render target that will exist throughout the whole game.
		// PP: this is a much nicer way of doing things if we can get away with it - deleting the VFX_renderTarget takes a lllooonnngggg time, eg. up to 75 scanlines on Xbox!
		VFX_bookPermanentRenderTarget();

// PP: currently in VFX_bookPermanentRenderTarget		VFX_useRenderTarget(VFX_NONE);
	}

	// PP: initialise stereoscopy
	VFX_initStereo();

	// PP: initialise shadow maps
	VFX_initShadowMap();

	// PP: initialise focus effects
	VFX_initFocus();
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initScene
	Purpose 	: Initilaisation to be done as each scene is prepared
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initScene(void)
{
	// PP: for the benefit of the stereoscopic mode, initialise current eye to 0
	VFX_stereoEye=0;

	// PP: we're not looking at the watch
	if(CheatActive(CHEAT_DIGITALWATCH))
	{
		watchLook=false;
	}

	// PP: setup motion blur...

	if(!CheatActive(CHEAT_MOTIONBLUR))
	{
		VFX_motionBlur(NORMAL_INGAME_MOTION_BLUR);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_update
	Purpose 	: udpate video effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_update(void) 
{
	if(VFX_effectActive(VFX_SPINOFF))
	{
		VFX_spinoffUpdate();
	}

	if(VFX_effectActive(VFX_FOCUS))
	{
		VFX_focusUpdate();
	}

	// PP: tick up timeslice timer - not much point making a function just for that, is there.
	VFX_timesliceTimer+=fTime;

	// PP: temp
/*	if(VFX_effectActive(VFX_STEREO))
	{
		VFX_stereoUpdate();
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_draw
	Purpose 	: normal draw for video effects
	Parameters 	: 
	Returns 	: 
	Info 		: some effects don't get drawn by this function becuase their draw
					has to be at some particular point, eg. before or after any
					other drawing
*/
void VFX_draw(void)
{
	/*if(VFX_effectActive(VFX_SPINOFF))
	{
		VFX_spinoffDraw();
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_shutdown
	Purpose 	: Shutdown video effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_shutdown(void)
{
	if(VFX_renderTarget != NULL)
	{
		bdDeleteRenderTarget(VFX_renderTarget);
	}
}


// PP: TEST


/*	--------------------------------------------------------------------------------
	Function 	: VFX_drawRenderTarget
	Purpose 	: Draw the effects render target to the screen
	Parameters 	: (opt/white)colour, (opt/default)flags to do with setting screen as render target,
					(opt/BLEND)blend mode (BDALPHABLENDMODE_), (opt/NULL=all) ptr to source rect (texture coords),
					(opt/NULL-whole viewport)ptr to destination rect, (opt/VFX_renderTarget) render target to draw,
					(opt/&VFX_renderTargetInfo)ptr to render target info, blah....
	Returns 	: 
	Info 		: 
*/

#define VFX_DRAWRENDERTARGET_DEFAULT_COLOUR			COLOUR(128, 128, 128, 128)
#define VFX_DRAWRENDERTARGET_DEFAULT_BLEND_MODE		BDALPHABLENDMODE_BLEND
#define VFX_DRAWRENDERTARGET_DEFAULT_FLAGS			BSETRENDERTARGET_CLEARZ

#define VFX_DRAWRENDERTARGET_VIEWPORT_RECT			(CURRENT_VIEWPORT.toOrigin())

static void VFX_drawRenderTarget(const COLOUR&					col					=VFX_DRAWRENDERTARGET_DEFAULT_COLOUR,
								 const int						blendMode			=VFX_DRAWRENDERTARGET_DEFAULT_BLEND_MODE,
								 const int						flags				=VFX_DRAWRENDERTARGET_DEFAULT_FLAGS,
								 const RECTANGLE* const			srcRectIn			=NULL,
								 const RECTANGLE* const			destRectIn			=NULL,
								 TBRenderTarget* const			renderTarget		=VFX_renderTarget,
								 const RENDERTARGETINFO* const	renderTargetInfo	=&VFX_renderTargetInfo,
								 const int						width				=VFX_RENDER_TARGET_WIDTH,
								 const int						height				=VFX_RENDER_TARGET_HEIGHT,
								 const bool						setRenderStates		=true)
{
	int			v;				// PP: loop counter for verts
	RECTANGLE	srcRect;		// PP: rectangle OF RENDER TARGET to draw to viewport
	RECTANGLE	destRect;		// PP: rectangle of viewport to which to draw render target

	// PP: set up source rect...
	
	// PP: correct for undersized render target
	srcRect.init(	0.0f,
					renderTargetInfo->width/(float)width,
					0.0f,
					renderTargetInfo->height/(float)height);

	// PP: use specified portion
	if(srcRectIn != NULL)
	{
		srcRect *= (*srcRectIn);
	}

	// PP: set up dest rect
	if(destRectIn == NULL)
	{
		destRect=VFX_DRAWRENDERTARGET_VIEWPORT_RECT;
	}
	else
	{
		destRect=(*destRectIn);
	}

	// ... render geometry to screen using target as texture ...

	TBPrimVertex	verts[4];

	if(setRenderStates)
	{
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

		// PP: is this needed?  I don't think so
		#if BPLATFORM == PS2
			bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		#endif	

		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	// PP: REMOUT: DON'T EVER USE BDRENDERSTATE_TEXTUREENABLE!!!!	bdSetRenderState(BDRENDERSTATE_TEXTUREENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, blendMode, 0);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, blendMode, 1);

#if(BPLATFORM == XBOX)// PP: TODO: TEST BY COMPARING RESOLUTIONS, NOT PLATFORM!!!!!!!!
		// PP: turn off filtering if the render target has exactly the same resolution as the frame buffer
// PP: REMOUT: not yet		SETFILTERING(POINT);
#endif// PP: (BPLATFORM == XBOX)
	}

	// set our render target as the current texture on stage zero
	bdSetRenderTargetAsTexture(renderTarget, 0);

	for(v=0; v<4; v++)
	{
		BDVERTEX_SETCOLOUR(&verts[v], col);
		BDVERTEX_SETXYZW(&verts[v], (v&1)?destRect.left:destRect.right, (v/2)?destRect.bottom:destRect.top, 0.0f, 0.0f);
		BDVERTEX_SETUV(&verts[v], (v&1)?srcRect.left:srcRect.right, (v/2)?srcRect.bottom:srcRect.top);
	}

	// PP: IMPORTANT!!! CAN'T WRAP A LINEAR TEXTURE SUCH AS A RENDER TARGET - MUST SET UV CLAMPING!!!
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 0);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);

	// remember to unset it as texture before we get back to rendering to it
	bdSetTexture(0, NULL);

	if(setRenderStates)
	{
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

#if(BPLATFORM == XBOX)// PP: TODO: TEST BY COMPARING RESOLUTIONS, NOT PLATFORM!!!!!!!!
		// PP: return filtering to whatever it is normally
		RESTOREFILTERING;
#endif// PP: (BPLATFORM == XBOX)
	}
}




/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												STEREOSCOPIC MODE

				Dig out those funky glasses, or make a pair with cardboard and some sweetie wrappers

							Did you know that this technique dates back to the 1850's?!
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* PP: RANT:

THERE IS NOTHING THAT I, OR ANYONE ELSE, CAN DO TO MAKE THIS EFFECT WORK BETTER.  YOU ARE VERY WELCOME TO TRY.

HERE ARE THE REASONS WHY YOU ARE GETTING DISSAPPOINTING RESULTS:

1. TAZ IS FULL OF BOLD VIVID COLOURS.  WHEN YOU MAKE AN ANAGLYPH YOU DO ***NOT*** USE AN IMAGE WITH BOLD VIVID COLOURS.
	TAZ IS THE WORST POSSIBLE GAME TO DEMONSTRATE ANAGLYPHIC STERESCOPY.

2. YOU ARE SEEING SOME RED IN YOUR CYAN EYE AND SOME GREEN AND BLUE IN YOUR RED EYE ("GHOSTING").
	WHILE THAT IS THE CASE, YOU ***CANNOT*** EXPECT GOOD RESULTS.  THE COLOURS I'M PUTTING ON THE SCREEN ARE THE RIGHT
	COLOURS.  WHAT THE TV DOES WITH THEM, AND WHAT YOUR 3D GLASSES THEN DO WITH THEM, IS OUTWITH MY CONTROL ENTIRELY.

IN ANOTHER GAME, PREFERABLY A MONOCHROME GAME DESIGNED WITH THE EFFECT IN MIND, THIS CODE WOULD GIVE MUCH BETTER RESULTS. 

BESIDES, IT'S JUST A BIT OF FUN.

  Aahhh, that's better.

*/

/* PP: 

Unfortunately, in the real world, ghosting (colour leakage) problems impose a harsh limit on the amount of depth that we can cleanly convey.
You can't let the two images get too far away from each other or the ghosting will make it all look cack.
You have to strike a good balance between IMPRESSION OF DEPTH and GENERAL VIEWABILITY.
That's why I'm using the same values on PC and consoles, and why I've used so much depth compression, and why I've used slightly more
compression on widescreen.
The world scale that I'm using is tweaked so that Taz is usually at the stereo convergence point, ie. he appears where your screen is.
This is becuase he's what you look at, so he's the thing that most deserves to have clean edges.  See?
I also keep text at the convergence point, because if you offset it, it looks cack - suffers very badly from ghosting.

*/

#define VFX_STEREO_INTERPUPILLARY_DISTANCE	CMTOUNIT(6.5f)		// PP: the physical distance between the player's eyes

// PP: NOTE: I realise that these values are a bit unrealistic for PC, but the end result is still the one I want - see?
#define VFX_STEREO_SCREEN_WIDTH				CMTOUNIT(41)		// PP: the physical width of the screen
#define VFX_STEREO_DISTANCE_FROM_SCREEN		CMTOUNIT(107)		// PP: the distance from the player's eyes to the screen											
			
// PP: NOTE: this combination of world scale

// PP: the scale of the world as regards stereoscopy.
// PP: I think this might be tied in with FOV?  We use quite a wide FOV.
// PP: Also, world scale can be used to falsely control where the stereo convergence point lands.
// PP: In this case, I keep it on Taz so that his outlines are kept clean (he's what you look at most)
// PP: twico
float worldscale=0.7f;
#define VFX_STEREO_WORLD_SCALE				worldscale

// PP: degree to which the depth range is compressed.
// PP: This is an unnatural adjustment to make the effect easier on the eyes.
// PP: Remember - focusing on the TV screen while looking way into the distance is not a natural or healthy thing to do!!!
// PP: Also, this helps keep the two images closer together (see above).
// PP: If you're thinking that it's dodgy for me to be using seperate 4:3/16:9 compressions, see comments above!
// PP: twico
float compress=0.8f;
float wscompress=0.83f;
#define VFX_STEREO_DEPTH_COMPRESSION		(gameStatus.wideScreenWilson?wscompress:compress)
																
// PP: borrow filter dimensions from the Glare stuff, why not...
#define VFX_STEREO_FILTER_WIDTH				GLARE_RECT_WIDTH	// PP: HALF the width of the filter quad
#define	VFX_STEREO_FILTER_HEIGHT			GLARE_RECT_HEIGHT	// PP: HALF the height of the filter quad

// PP: do we use the VFX_renderTarget for the stereo effect?  We don't have to you know...
bool					VFX_stereoUseRenderTarget;

EAnaglyphType			VFX_stereoAnaglyphType=ANAGLYPH_RED_CYAN;	// PP: the type of anaglyph to be generated by the stereoscopic mode

// PP: colours of each of the eyes
// PP: =======================================================
// PP: *** NOTE!: I DON'T THINK THESE ARE USED ANYMORE!!!!!!!!
// PP: =======================================================
static COLOUR			VFX_stereoColours[2]={
												COLOUR(0, 1, 1, 1),	// PP: right eye CYAN
												COLOUR(1, 0, 0, 1)	// PP: left eye RED
											};// PP: OOD!

static TBPrimVertex		VFX_stereoFilterVerts[4];				// PP: precalc'ed vertices for the filter

// PP: colour channels for each of the eyes' draw passes, for each anaglyph type
static const uint32		VFX_stereoColourChannels[NUM_ANAGLYPH_TYPES][2]={{BDFRAMEWRITE_BLUE|BDFRAMEWRITE_GREEN,	BDFRAMEWRITE_RED},
																		{BDFRAMEWRITE_RED,						BDFRAMEWRITE_BLUE|BDFRAMEWRITE_GREEN}};
															
int						VFX_stereoEye;							// PP: which eye's view is being drawn?

// PP: factors used to adjust the anaglyph's colour channel strengths, to reduce ghosting
// PP: OOD, NOT USED!
static struct
{
	float	red;
	float	green;
	float	blue;
}						VFX_stereoColourStrengths={1.0f, 1.0f, 1.0f};


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initStereo
	Purpose 	: Initialise stereoscopic mode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initStereo(void)
{
	// PP: Set up filter poly...

	COLOUR	col;

	col=VFX_stereoColours[0].inverse(1)*255;//COLOUR(255,255,255,255);//
	col.a=255;

	for(int v=0; v<4; v++)
	{
		BDVERTEX_SETCOLOUR(&VFX_stereoFilterVerts[v], col);// PP: filter is only used for first eye

		BDVERTEX_SETXYZW(	&VFX_stereoFilterVerts[v],
							((v&1)?(-1.0f):(1.0f))*VFX_STEREO_FILTER_WIDTH,
							((v/2)?(-1.0f):(1.0f))*VFX_STEREO_FILTER_HEIGHT,
							0.0f,
							0.0f);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_setStereo
	Purpose 	: turn the stereoscopic mode on or off
	Parameters 	: on/off, de-saturate colours, (opt/false) priority flag
	Returns 	: 
	Info 		: looks silly I know, but makes sense I think

				*** NOTE: the 'priority' flag is used to indicate that the stereo vision is being activated/dectivated by a stereo vision cheat.
					When using stereo vision (for whatever wacky reason) within the game, please leave the priority flag with its default value of false.
*/
void VFX_setStereo(const bool on, const bool grey, const bool priority)
{
	if(on)
	{
		if(!VFX_effectActive(VFX_STEREO))
		{
			// PP: we want to turn stereo vision on

			// PP: record whether or not we want to de-saturate
			VFX_stereoGrey=grey;

			// PP: if we want colour stereoscopy, and we have channel-masking hardware, don't use the VFX_renderTarget

			if(COLOUR_CHANNEL_MASKING_SUPPORTED)
			{
				VFX_stereoUseRenderTarget=VFX_stereoGrey;
			}
			else// PP: if COLOUR_CHANNEL_MASKING_SUPPORTED else
			{
				VFX_stereoUseRenderTarget=true;
			}

			// PP: is the VFX_renderTarget being used by another effect?

			if(VFX_stereoUseRenderTarget && (VFX_renderTargetUser == VFX_MOTIONBLUR))
			{
				// PP: aye, it's being used for motion blur

				if(priority)
				{
					// PP: high priority - we can turn the motion blur cheat off
					if(!DeactivateCheat(CHEAT_MOTIONBLUR))
					{
						// PP: motion blur cheat wasn't on - turn the effect off
						VFX_deactivateEffect(VFX_MOTIONBLUR);
					}

					VFX_activateEffect(VFX_STEREO, priority);
				}
				else
				{
					if(!CheatActive(CHEAT_MOTIONBLUR))
					{
						// PP: use the latest effect requested
						VFX_deactivateEffect(VFX_MOTIONBLUR);
						VFX_activateEffect(VFX_STEREO, priority);
					}
				}
			}
			else
			{
				// PP: render target is free - go for it
				VFX_activateEffect(VFX_STEREO);
			}
		}
	}
	else// PP: trying to deactivate
	{
		if(VFX_effectActive(VFX_STEREO))
		{
			// PP: if a stereoscopic cheat is enabled, you need the priority flag to disable stereo vision
			if((!(CheatActive(CHEAT_COLOURSTEREOSCOPIC)||CheatActive(CHEAT_GREYSTEREOSCOPIC))) || priority)
			{
				VFX_deactivateEffect(VFX_STEREO);
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_stereoPreDraw
	Purpose 	: Prepare to draw a scene stereoscopically
	Parameters 	: 
	Returns 	: 
	Info 		: (second eye only) sets the VFX_renderTarget as the current render target
*/
void VFX_stereoPreDraw(void)
{
	if(VFX_stereoUseRenderTarget)
	{
		if(VFX_stereoEye)
		{
			// PP: set the VFX_renderTarget as the current render target
			pushRenderTarget(VFX_renderTarget, 0,0,0,255, 1.0f, BSETRENDERTARGET_CLEAR);
		}
		else// PP: first eye renders straight to the screen
		{
			// switch back to the primary target, note we need to clear the Z buffer as our shared target will have modified it
			popRenderTarget(0,0,0,0, 1.0f, BSETRENDERTARGET_CLEAR);
		}
	}

#if(BPLATFORM == PC)
			if(bDisplayInfo.devCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE)
#else
			if(COLOUR_CHANNEL_MASKING_SUPPORTED)
#endif
	{
		if(VFX_stereoEye)
		{
			// PP: **** RIGHT EYE ****

			// PP: clear the z-buffer
			bdClear(0,0,0,255,1.0f,0,BDCLEARMODE_Z);

			if(VFX_stereoGrey)
			{
				// PP: if we're in 'grey' mode, we only need the red channel into the RT and we'll shift it across to the other channels
				bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 0);
				bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 1);
			}
			else
			{
				// PP: only draw to the right eye's colour channels
				rgbFramewriteChannels=VFX_stereoColourChannels[VFX_stereoAnaglyphType][1];

				bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 0);
				bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 1);
			}
		}
		else
		{
			rgbFramewriteChannels=VFX_stereoColourChannels[VFX_stereoAnaglyphType][0];

			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 0);
			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 1);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_colourRenderTarget
	Purpose 	: Copy the left eye's colour component on the right eye's render target
					into the colour component(s) of the right eye, thus removing
					brightness discrepancies between the two eyes, see?
	Parameters 	: 
	Returns 	: 
	Info 		: USE THIS ON THE SECOND EYE ONLY
				// PP: TODO: 'cyan-grey'(slower) and 'average grey'(slowest) modes?  See how it goes once the zbufferisms are sorted.  get it - sorted?!!  // PP: TODO: talk to girls more
*/
void VFX_colourRenderTarget(void)
{

// PP: IF0OUT: NEEDS ATTENTION
#if 0

#if (VFX_RENDER_TARGET_COLOUR_DEPTH == 32)

// PP: NOTE: 32 BIT VERSION CRASHES FOR SOME STRANGE REASON.  DO WE REALLY NEED 32 BIT COLOUR ANYWAY?  IT LOOKS OK IN 16 TO ME.

#define COLOUR_TYPE		ulong

#define VLOOP \
		col= *ptr; /*PP: get colour*/ \
		r=col&0xff;/* PP: get red*/ \
		newCol=(r<<8);/* PP: copy red into green */ \
		newCol|=(r<<16);/* PP: copy red into blue */ \
		newCol|=0x80000000;/* PP: TEST: set alpha on*/ \
		*ptr=newCol; \
		\
		ptr++;

#else// PP: 16

#define COLOUR_TYPE		ushort

#define VLOOP \
		col= *ptr; /*PP: get colour*/ \
		r=col&0x1f;/* PP: get red*/ \
		newCol=(r<<5);/* PP: copy red into green */ \
		newCol|=(r<<10);/* PP: copy red into blue */ \
		newCol|=0x8000;/* PP: TEST: set alpha bit high*/ \
		*ptr=newCol; \
		\
		ptr++;

#endif// PP: (VFX_RENDER_TARGET_COLOUR_DEPTH == 32) else


	COLOUR_TYPE*			ptr=NULL;
	register char			r/*,g,b*/;
	register COLOUR_TYPE	col, newCol;
	register int			h,v;
	register int			max_h, max_v;

	// PP: lock the render target... // PP: TODO: **** PC SURFACE LOCK ****
#if (BPLATFORM == PS2)
	int		xDim, yDim, pitch, rgbBits;
	ptr=(COLOUR_TYPE*)bdLockRenderTarget(VFX_renderTarget, &xDim, &yDim, &pitch, &rgbBits);
#endif// PP: (BPLATFORM == PS2)

	// PP: can't do nuffin if it's null Squire
	if(ptr == NULL) return;


#define HLOOP_UNROLL	8
#define VLOOP_UNROLL	8

	max_h=VFX_renderTargetInfo.width/HLOOP_UNROLL;	// PP: PROBLEM - width may not be divisible
	max_v=VFX_renderTargetInfo.height/VLOOP_UNROLL;	// PP: PROBLEM - height may not be divisible


#define HLOOP \
		for(h=0; h<max_h; h++) \
		{ \
			for(v=0; v<max_v; v++) \
			{ \
				VLOOP; \
				VLOOP; \
				VLOOP; \
				VLOOP; \
				\
				VLOOP; \
				VLOOP; \
				VLOOP; \
				VLOOP; \
			} \
		}

	for(h=0; h<max_h; h++)
	{
		HLOOP;
		HLOOP;
		HLOOP;
		HLOOP;

		HLOOP;
		HLOOP;
		HLOOP;
		HLOOP;
	}

	// PP: unlock the render target// PP: TODO: **** PC SURFACE UNLOCK ****
#if (BPLATFORM == PS2)
	bdUnlockRenderTarget(VFX_renderTarget);
#endif// PP: (BPLATFORM == PS2)

#endif// PP: 0
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_stereoPostDraw
	Purpose 	: render the stereoscopic image
	Parameters 	: 
	Returns 	: true if the draw loop is to be repeated
	Info 		: 
*/
bool VFX_stereoPostDraw(void)
{
	bool	rtn;

	if(VFX_stereoEye)
	{
		VFX_stereoEye/*=videoMode.drawNum*/=0;// PP: switch eyes

		if(VFX_stereoUseRenderTarget)
		{
			bdBeginScene();

			// PP: finished rendering the scene for both eyes - now whack it onto the screen as a stereoscopic image
			bdSetIdentityObjectMatrix();

			// PP: de-saturate if we're supposed to
			if(VFX_stereoGrey)
			{
				VFX_colourRenderTarget();
			}

			VFX_drawRenderTarget(VFX_stereoColours[1]*128, BDALPHABLENDMODE_ADDITIVE);

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdEndScene();
		}

		rtn=false;
	}
	else
	{
		VFX_stereoEye/*=videoMode.drawNum*/=1;// PP: switch eyes

		rtn=true;
	}

	// PP: fix colour strengths, thus reducing ghosting
	// PP: REMOUT: doesn't reduce ghosting at all!	D'oh!	VFX_applyStereoColourStrengths();

	return rtn;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_adjustStereoCamera
	Purpose 	: Adjust a camera's position and orientation for stereoscopic effect
	Parameters 	: [in/out] ref to camera position VEC, [in/out] camera orientation quaternion
	Returns 	: 
	Info 		: 
*/
void VFX_adjustCameraForStereoscopy(VEC& camPos, TBQuaternion camQuat)
{
	VEC				posOffset;
	float			offsetDist;
	float			convergeAng;
	float			convergeAng2;	// PP: NOT the same as convergeAng
	TBQuaternion	tempQuat;
	TBQuaternion	convergeQuat;

	// PP: calculate camera offset, based on viewer's distance in screen-widths from the screen...

	// PP: The convergence angle of each of the two cameras is to the game's horizontal FOV as the interpupillary distance is to the screen width.  Can't remember why exactly...
	convergeAng=((VFX_STEREO_INTERPUPILLARY_DISTANCE*(1.0f-VFX_STEREO_DEPTH_COMPRESSION))/VFX_STEREO_SCREEN_WIDTH)*(CURRENT_SCREEN.horzFov*0.5f);

	offsetDist=bmTan(convergeAng)*(VFX_STEREO_DISTANCE_FROM_SCREEN);

	// PP: shift in correct direction for current eye
	if(VFX_stereoEye)
	{
		offsetDist= -offsetDist;
	}

	posOffset=VEC(offsetDist/VFX_STEREO_WORLD_SCALE, 0.0f, 0.0f);

	// PP: rotate the position offset by the camera's angle quat

	TBMatrix	tempMatrix;
	
	bmQuatToMatrix(tempMatrix, camQuat);
	bmMatMultiplyVector(tempMatrix, posOffset);

	// PP: offset cam pos
	camPos+=posOffset;

	// PP: get a quat representing the Y angle of convergence for this eye (convergeQuat)...

	convergeAng2=bmATan2(offsetDist, VFX_STEREO_DISTANCE_FROM_SCREEN/VFX_STEREO_WORLD_SCALE);

	TBVector	rotationVec;

	SETVECTOR(rotationVec, 0.0f, 1.0f, 0.0f, convergeAng2);

	bmRotationToQuat(convergeQuat, rotationVec);

	// PP: combine the camera angle quat with the stereo convergence quat

	bmQuatMultiply(tempQuat, convergeQuat, camQuat);

	bmQuatCopy(camQuat, tempQuat);
}


/*	--------------------------------------------------------------------------------
	Function 	: DrawStereoscopyFilter
	Purpose 	: Apply a colour filter over EVERYTHING that has been drawn,
					allowing through only the colour of the eye
	Parameters 	: 
	Returns 	: 
	Info 		: USE THIS ON THE FIRST EYE ONLY
*/
void VFX_drawStereoFilter(void)
{
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();
										 
	#if BPLATFORM == PS2
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	#endif	

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 1);
	
	bdSetTexture(0,NULL);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,VFX_stereoFilterVerts,4,BVERTTYPE_SINGLE);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_applyStereoColourStrengths
	Purpose 	: alter the strength(s) of the colour channel(s) of this eye's
					draw pass; thus reducing ghosting
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: NOTE: I'M NOT USING THIS FUNCTION NOW, BECAUSE IT DOESN'T REDUCE GHOSTING AT ALL!!
*/
void VFX_applyStereoColourStrengths(void)
{
	SETPROJECTION(2D);
	bdSetIdentityObjectMatrix();

	SAVESTATES;

	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);
	NOTEXTURE;
	ENABLEBLEND(TRUE);
	SETBLEND(SUBTRACTIVE);

	TBPrimVertex	verts[4];

	for(int v=0; v<4; v++)
	{
		BDVERTEX_SETXYZW(	&verts[v],
							((v&1)?(-1.0f):(1.0f))*VFX_STEREO_FILTER_WIDTH,
							((v/2)?(-1.0f):(1.0f))*VFX_STEREO_FILTER_HEIGHT,
							0.0f,
							0.0f);

		BDVERTEX_SETRGBA(	&verts[v],
							bmFloatToInt((1.0f-VFX_stereoColourStrengths.red)*255),
							bmFloatToInt((1.0f-VFX_stereoColourStrengths.green)*255),
							bmFloatToInt((1.0f-VFX_stereoColourStrengths.blue)*255),
							255);
	}

	// PP: duh, imagine forgetting this line!
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);

	RESTORESTATES;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MOTION BLUR

							Full screen only at the moment, I don't know if we need anything else?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static bool				VFX_motionBlurReady;			// PP: render target has been written to, and is now ready to be pasted onto the screen

// PP: define MOTIONBLUR_DEBUG to get debug messages about motion blur
//#define MOTIONBLUR_DEBUG


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlur
	Purpose 	: Add to the motion blur level for this 
	Parameters 	: level of motion blur to add: 0.0f(none [ignored])..1.0f(all [image doesn't update])
	Returns 	: 
	Info 		: // PP: motion blur level is set to zero at the beginning of each frame,
					and is then added to by any number of things throughout the frame, using this function
*/
void VFX_motionBlur(const float blur)
{
	// NH: EEEEEKK!!!! Motion blur seems to screw up (crash) PC's.  So if u have a PC OFF, your not getting blurs (Well not till it's sorted!)
#if BPLATFORM == PC
	return;
#endif

	if(videoMode.numDraws>1)
	{
		// PP: motion blur gets a bit more complicated when there's more than one draw pass
		return;
	}

	if(blur <= 0.0f)
	{
		// PP: ye can't use this function to decrease the motion blur level.
		// PP: no reason why that couldn't be the case, just that I don't think we're gonna need it.

		// PP: and if we're adding 0.0f onto the level, we can jolly well sod off too.
		return;
	}

	// PP: milestone bodge - no motion blur in splitscreen - boo-hoo!
#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET
	if(gameStatus.multiplayer.numScreens > 1)
	{
		return;
	}
#endif// PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

	if(!VFX_effectActive(VFX_MOTIONBLUR))
	{
		// PP: we want to turn motion blur on

		VFX_activateEffect(VFX_MOTIONBLUR);

		// PP: *** NOTE *** : I'm using flags from the render-target version of the effect even if the RT doesn't get used.
		// PP:					This gives a bit of consistency in the way the effect works across the different platforms.

		// PP: VFX_renderTarget hasn't been written to yet; don't paste it onto the screen at the end of this frame
		VFX_motionBlurReady=false;

		// PP: we haven't asked the effect to stop yet.  Hell, it's only just started!
		VFX_motionBlurSwitchOffPlease=false;
	}

	VFX_motionBlurLevel+=blur;

#if (BPLATFORM == PS2)

	// PP: clamp motion blur level to 1.0f
	if(VFX_motionBlurLevel > 1.0f)
	{
		VFX_motionBlurLevel=1.0f;
	}

	// PP: hmm - can't this just be done in postdraw?
	VFX_motionBlurAlpha=(unsigned char)((1.0f-VFX_motionBlurLevel)*128);

	bDisplayInfo.motionBlurVal=VFX_motionBlurAlpha;

#endif // PP: BPLATFORM == PS2

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlurPreDraw
	Purpose 	: Prepare to draw with motion blur
	Parameters 	: 
	Returns 	: 
	Info 		: (platforms other than PS2) sets the VFX_renderTarget as the current render target
*/
void VFX_motionBlurPreDraw(void)
{
	// PP: Set the motion blur level to zero at the beginning of each frame
	VFX_motionBlurLevel=0.0f;

	VFX_motionBlurAlpha=128;

#if (BPLATFORM == PS2)

	bDisplayInfo.motionBlurVal=128;

#endif // PP: BPLATFORM == PS2

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET

	// PP: set the VFX_renderTarget as the current render target
	pushRenderTarget(VFX_renderTarget, 0,0,0,255, 1.0f, BSETRENDERTARGET_CLEAR);

#ifdef MOTIONBLUR_DEBUG
	bkPrintf("------>>	VFX_motionBlurPreDraw: pushing render target\n");
#endif// PP: MOTIONBLUR_DEBUG

	// PP: when we next reach post-draw, the render target will have been written to, and will be ready to be pasted onto the screen
	VFX_motionBlurReady=true;

#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlurPostDraw
	Purpose 	: draw with motion blur, having rendered the scene
	Parameters 	: 
	Returns 	: 
	Info 		: (platforms other than PS2) blends the current frame onto the previous frames
*/
void VFX_motionBlurPostDraw(void)
{
	if(!VFX_effectActive(VFX_MOTIONBLUR))
	{
		return;
	}

	if(!VFX_motionBlurReady)
	{
		return;
	}

	if(VFX_motionBlurLevel == 0.0f)
	{
		// PP: no motion blur requested this frame - deactivate effect
		VFX_deactivateEffect(VFX_MOTIONBLUR);
	}

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET

#if(BPLATFORM == XBOX)
	// PP: Xbox version seems to be triple-buffered rather than double-buffered like the PS2
	// PP: so boost motion blur value to compensate
	VFX_motionBlurLevel *= 1.5f;// PP: 3/2
#endif// PP: xbox

#define VFX_MAX_MOTION_BLUR_LEVEL	0.8f

	// PP: clamp motion blur level to max
	if(VFX_motionBlurLevel > VFX_MAX_MOTION_BLUR_LEVEL)
	{
		VFX_motionBlurLevel=VFX_MAX_MOTION_BLUR_LEVEL;
	}

	VFX_motionBlurAlpha=(unsigned char)((1.0f-VFX_motionBlurLevel)*128);

	// PP: duh this is kinda important Phil
	popRenderTarget();

#ifdef MOTIONBLUR_DEBUG
	bkPrintf("<<------	VFX_motionBlurPostDraw: popping render target\n");
#endif// PP: MOTIONBLUR_DEBUG

	// PP: draw the render target to the screen at appropriate alpha...

	bdSetIdentityObjectMatrix();

	VFX_drawRenderTarget(COLOUR(128, 128, 128, VFX_motionBlurAlpha));

#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														VIDEO ZOOM

										To do?  I can't think of a good use for it.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: VFX_setZoom
	Purpose 	: Set the level of 2D zoom to use
	Parameters 	: (opt/1.0f=off) magnification factor of 2D zoom
	Returns 	: 
	Info 		: I think this'll need to use the zoom factor exponentially to control width & height of render target poly
*/
void VFX_setZoom(const float zoom)
{
	// PP: TODO
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															BLUR

	Renders the scene onto a small texture area, which when scaled up over the whole screen, filters and causes a blur.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


static float	VFX_blur;


/*	--------------------------------------------------------------------------------
	Function 	: VFX_setBlur
	Purpose 	: Set the level of blurring to use
	Parameters 	: (opt/0.0f=off) level of blur
	Returns 	: 
	Info 		: 
*/
void VFX_setBlur(const float blur)
{
	VFX_blur=blur;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											BLURRED CHARACTER SHADOW MAPS

													PS2-friendly
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: Define this to get softer shadows.
// PP: Leave it undefined to get sharper shadows
//#define BLUR_SHADOWMAPS

// PP: still can't get that goddam dither off
//#define VFX_SHADOWMAP_DITHERING

// PP: BLUR PARAMETERS ~~~~~~~~~

#ifdef VFX_SHADOWMAP_DITHERING
#define SHADOWMAP_SUBTRACTIVE_BLUR_TOTAL_INTENSITY		128		// PP: combined intensity of all blur layers
#define SHADOWMAP_BLEND_BLUR_TOTAL_INTENSITY			400		// PP: combined intensity of all blur layers
#define SHADOWMAP_BLUR_RADIUS							0.0075f//0.009f	// PP: radius of blur, as a texture coordinate distance on the source map
#else
#define SHADOWMAP_SUBTRACTIVE_BLUR_TOTAL_INTENSITY		128//128		// PP: combined intensity of all blur layers
#define SHADOWMAP_BLEND_BLUR_TOTAL_INTENSITY			400		// PP: combined intensity of all blur layers
#define SHADOWMAP_BLUR_RADIUS							0.026f	// PP: radius of blur, as a texture coordinate distance on the source map
#endif

// PP: TODO:   PLAY ABOUT WITH THIS PARAMETER, COMPARE THE SMOOTHNESS, AND COMPARE THE DRAW TIMES.
// PP: ******************************************
#ifdef VFX_SHADOWMAP_DITHERING
#define SHADOWMAP_NUM_BLUR_STEPS				1
#else
#define SHADOWMAP_NUM_BLUR_STEPS				4
#endif
// PP: ******************************************

// PP: (4 layers * 4 verts) per blur step
#define SHADOWMAP_NUM_BLUR_VERTS				(SHADOWMAP_NUM_BLUR_STEPS*16)

// PP: precalc'd verts for blur stage (4 offset overlays)
static TBPrimVertex								VFX_shadowSubtractiveBlurVerts[SHADOWMAP_NUM_BLUR_VERTS];	// PP: for subtractive shadows
static TBPrimVertex								VFX_shadowBlendBlurVerts[SHADOWMAP_NUM_BLUR_VERTS];			// PP: for blended shadows

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~
																				
// PP: borrow filter dimensions from the Glare stuff, why not...				
#define VFX_SHADOWMAP_BACKGROUND_WIDTH			VFX_STEREO_FILTER_WIDTH				// PP: HALF the width of the filter quad
#define	VFX_SHADOWMAP_BACKGROUND_HEIGHT			VFX_STEREO_FILTER_HEIGHT			// PP: HALF the height of the filter quad
									
#ifdef BLUR_SHADOWMAPS			
#define VFX_SHADOW_RENDER_TARGET_WIDTH			128
#define VFX_SHADOW_RENDER_TARGET_HEIGHT			128
#define VFX_SHADOW_RENDER_TARGET_DEPTH			32// PP: 16
#else// PP: no blur
#if(BPLATFORM == XBOX)// PP: "Because I'm worth it!"™

#define VFX_SHADOW_RENDER_TARGET_WIDTH			512
// TP: NOTE: no render target can be any bigger than the screen regardless of whether you are using a private z or not
#define VFX_SHADOW_RENDER_TARGET_HEIGHT			480

#else// PP: !xbox
#define VFX_SHADOW_RENDER_TARGET_WIDTH			256
#define VFX_SHADOW_RENDER_TARGET_HEIGHT			256
#endif// PP: !xbox
#define VFX_SHADOW_RENDER_TARGET_DEPTH			16
#endif// PP: no blur


												
TBRenderTarget*									VFX_shadowRenderTarget=NULL;		// PP: final render target containing blurred shadow map
#ifdef BLUR_SHADOWMAPS
static TBRenderTarget*							VFX_tempShadowRenderTarget=NULL;	// PP: temporary render target containing raw shadow map
#endif// PP: def BLUR_SHADOWMAPS
RENDERTARGETINFO								VFX_shadowRenderTargetInfo;			// PP: NOT CURRENTLY USED, kept here just in case, yadda yadda, debug, yadda...
												
TBActRenderMode_Outlined						VFX_shadowLineParms;				// PP: parameters for the outline rendering

#define VFX_SHADOWMAP_NUM_RENDERMODE_STAGES		2									// PP: number of actor render modes to copy & restore after generating the shadow map

/*	--------------------------------------------------------------------------------
	Function 	: VFX_initShadowMap
	Purpose 	: initialise shadow maps
	Parameters 	: 
	Returns 	: 
	Info 		: sets up verts for blur stage
*/
void VFX_initShadowMap(void)
{
#ifdef BLUR_SHADOWMAPS
	// PP: set up the 4 offset overlayed quads that will form the blur filter for the shadow...

	int				vertIndex=0;
	float			uOffset, vOffset;
	int				blurLayers=SHADOWMAP_NUM_BLUR_STEPS*4;// PP: 4 layers per blur step
	uchar			subtractiveLayerIntensity=SHADOWMAP_SUBTRACTIVE_BLUR_TOTAL_INTENSITY/blurLayers;
	uint32			blendLayerIntensity=SHADOWMAP_BLEND_BLUR_TOTAL_INTENSITY/blurLayers;
	float			layerOffset;

	for(int i=0; i<blurLayers; i++)
	{
		layerOffset=((1+(i/4))/(float)SHADOWMAP_NUM_BLUR_STEPS)*SHADOWMAP_BLUR_RADIUS;// PP: 4 layers per blur step
		uOffset=(i&1)?layerOffset:(-layerOffset);
		vOffset=(i/2)?layerOffset:(-layerOffset);

		for(int j=0; j<4; j++)
		{
			BDVERTEX_SETRGBA(&VFX_shadowSubtractiveBlurVerts[vertIndex], subtractiveLayerIntensity, subtractiveLayerIntensity, subtractiveLayerIntensity, 30/*subtractiveLayerIntensity*/);
			BDVERTEX_SETUV(&VFX_shadowSubtractiveBlurVerts[vertIndex], (j&1)?uOffset:(1.0f+uOffset), (j/2)?vOffset:(1.0f+vOffset));
			
			// PP: PLATFORM: Answers on a postcard!
#if(BPLATFORM == PS2)
			BDVERTEX_SETXYZW(&VFX_shadowSubtractiveBlurVerts[vertIndex], ((j&1)?(-0.5f):(0.5f))*VFX_SHADOW_RENDER_TARGET_WIDTH, ((j/2)?0.5f:(-0.5f))*VFX_SHADOW_RENDER_TARGET_HEIGHT, 0.0f, 0.0f);
#else// PP: if(BPLATFORM == PS2) else
			BDVERTEX_SETXYZW(&VFX_shadowSubtractiveBlurVerts[vertIndex], ((j&1)?(-2.0f):(2.0f))*VFX_SHADOW_RENDER_TARGET_WIDTH, ((j/2)?2.0f:(-2.0f))*VFX_SHADOW_RENDER_TARGET_HEIGHT, 0.0f, 0.0f);
#endif// PP: (BPLATFORM == PS2) else
			
			// PP: ~~~~~~~~~

			BDVERTEX_SETRGBA(&VFX_shadowBlendBlurVerts[vertIndex], 128, 128, 128, blendLayerIntensity);
			BDVERTEX_SETUV(&VFX_shadowBlendBlurVerts[vertIndex], (j&1)?uOffset:(1.0f+uOffset), (j/2)?vOffset:(1.0f+vOffset));
			
			// PP: PLATFORM: Answers on a postcard!
#if(BPLATFORM == PS2)
			BDVERTEX_SETXYZW(&VFX_shadowBlendBlurVerts[vertIndex], ((j&1)?(-0.5f):(0.5f))*VFX_SHADOW_RENDER_TARGET_WIDTH, ((j/2)?0.5f:(-0.5f))*VFX_SHADOW_RENDER_TARGET_HEIGHT, 0.0f, 0.0f);
#else// PP: if(BPLATFORM == PS2) else
			BDVERTEX_SETXYZW(&VFX_shadowBlendBlurVerts[vertIndex], ((j&1)?(-2.0f):(2.0f))*VFX_SHADOW_RENDER_TARGET_WIDTH, ((j/2)?2.0f:(-2.0f))*VFX_SHADOW_RENDER_TARGET_HEIGHT, 0.0f, 0.0f);
#endif// PP: (BPLATFORM == PS2) else

			// PP: ~~~~~~~~~

			vertIndex++;
		}
	}
#endif// PP: def BLUR_SHADOWMAPS

	// PP: set up outline parameters - the outline render mode is currently the only way I can coax Babel into doing this thing.
	// PP: it does have the bonus of allowing the silhouette to be thickened (doesn't bother me much)
	// PP: NOTE: I don't imagine it's the fastest way though!!

	VFX_shadowLineParms.thickness = 0.0f;// PP: I wonder if that gives any speedup?
	VFX_shadowLineParms.r = 255;
	VFX_shadowLineParms.g = 255;
	VFX_shadowLineParms.b = 255;
	VFX_shadowLineParms.a = 255;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_createShadowRenderTarget
	Purpose 	: create the render target for shadow maps
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_createShadowRenderTarget(void)
{
	// PP: final render target to contain blurred shadow map...

	if(VFX_shadowRenderTarget == NULL)
	{
		// create a target with VFX_SHADOW_RENDER_TARGET_DEPTH-bit colour & z
		// NH: NOTE: On PC the Depth and the z MUST be the same
		if (VFX_shadowRenderTarget = bdCreateRenderTarget(VFX_SHADOW_RENDER_TARGET_WIDTH, VFX_SHADOW_RENDER_TARGET_HEIGHT, VFX_SHADOW_RENDER_TARGET_DEPTH, VFX_SHADOW_RENDER_TARGET_DEPTH, BCREATERENDERTARGET_NODITHER|BCREATERENDERTARGET_PRIVATEZ))// PP: private z: new test
		{
			// retrieve the dimensions and depths that were actually used
			bdGetRenderTargetInfo(VFX_shadowRenderTarget,
								&(VFX_shadowRenderTargetInfo.width),
								&(VFX_shadowRenderTargetInfo.height),
								&(VFX_shadowRenderTargetInfo.rgbDepth),
								&(VFX_shadowRenderTargetInfo.zDepth));
		}
		else
		{
			bkPrintf("VFX_createShadowRenderTarget: failed to create VFX_shadowRenderTarget\n");
		}
	}

	// PP: temporary render target to contain raw shadow map...

#ifdef BLUR_SHADOWMAPS

	if(VFX_tempShadowRenderTarget == NULL)
	{
		// create a target with 16-bit colour & Z
		if (NULL == (VFX_tempShadowRenderTarget = bdCreateRenderTarget(VFX_SHADOW_RENDER_TARGET_WIDTH, VFX_SHADOW_RENDER_TARGET_HEIGHT, VFX_SHADOW_RENDER_TARGET_DEPTH, VFX_SHADOW_RENDER_TARGET_DEPTH, BCREATERENDERTARGET_NODITHER|BCREATERENDERTARGET_PRIVATEZ)))// PP: private z: new test
		{
			bkPrintf("VFX_createShadowRenderTarget: failed to create VFX_tempShadowRenderTarget\n");
		}
	}

#endif// PP: def BLUR_SHADOWMAPS
}



/*	--------------------------------------------------------------------------------
	Function 	: VFX_releaseShadowRenderTarget
	Purpose 	: release the VFX_shadowRenderTarget
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_releaseShadowRenderTarget(void)
{
	if(VFX_shadowRenderTarget != NULL)
	{
		// PP: NOTE: It is essential that the specified render target not be current on any texture stage when deleted, if this occurs a crash is likely.

		bdSetTexture(0, NULL);

		bdDeleteRenderTarget(VFX_shadowRenderTarget);

		VFX_shadowRenderTarget=NULL;
	}

#ifdef BLUR_SHADOWMAPS

	if(VFX_tempShadowRenderTarget != NULL)
	{
		// PP: NOTE: TT: It is essential that the specified render target not be current on any texture stage when deleted, if this occurs a crash is likely.

		bdSetTexture(0, NULL);

		bdDeleteRenderTarget(VFX_tempShadowRenderTarget);

		VFX_tempShadowRenderTarget=NULL;
	}

#endif// PP: def BLUR_SHADOWMAPS
}


#ifdef BLUR_SHADOWMAPS
/*	--------------------------------------------------------------------------------
	Function 	: VFX_blurShadowMap
	Purpose 	: blur the shadow map
	Parameters 	: ptr to the light that cast the shadow
	Returns 	: 
	Info 		: EAT MY BLUR.  NO REALLY, EAT MY BLUR.  SMOOTH AS A GODDAM IAN!!!
*/
static void VFX_blurShadowMap(const SHADOWLIGHT* const light)
{
	if(VFX_tempShadowRenderTarget == NULL) return;// PP: no can do if null

	COLOUR	clearCol;

	// PP: determine colour to which to clear the render target
	if(light->blend == SHADOWBLEND_BLEND)
	{
		// PP: start from transparent (any rgb) and ADD each blur layer
		clearCol=COLOUR(0,0,0,0);
	}
	else// PP: subtractive
	{
		clearCol=COLOUR(0,0,0,255);
	}

	// PP: NOTE: I'm not pushing this render target onto the stack, cos otherwise when I popRenderTarget it'd only take me back to the VFX_tempshadowRenderTarget!!
	bdSetRenderTarget(VFX_shadowRenderTarget, clearCol.r, clearCol.g, clearCol.b, clearCol.a, 1.0f, BSETRENDERTARGET_CLEARRGB);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	// PP: set our render target as the current texture on stage zero
	bdSetRenderTargetAsTexture(VFX_tempShadowRenderTarget, 0);

	ASSERT(SHADOWMAP_NUM_BLUR_VERTS);

	if(light->blend == SHADOWBLEND_BLEND)
	{

#define AW_JINGS_IT_WORKS_WITH_NORMAL_ADDITIVE_AFTER_ALL

#ifndef AW_JINGS_IT_WORKS_WITH_NORMAL_ADDITIVE_AFTER_ALL

		// PP: set [destCol+(srcCol*srcAlpha)] blending...
		// PP: TODO: If this works on all platforms I'll ask Tech Team if they can put it into Babel.
		// PP: It's a nice blend y'know!  Could be very useful for something at some point, somewhere, possibly...

#if(BPLATFORM == XBOX)

	D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
	D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

#elif(BPLATFORM == PC)

	bDisplayInfo.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

#elif(BPLATFORM == PS2)

// PP: if you need to rem these errors out for just now, don't worry about it.
// PP: It shouldn't break anything apart from the Zoo Boss shadows.
#error "PP: Hi, could we have [destCol+(srcCol*srcAlpha)] blending here please - it should work with this formula:"
#error "PP: destCol=(srcCol-0)*srcAlpha+destCol."
// PP: this comforms to the PS2 blending formula // PP: PHEW

/* PP: REMINDER: PS2 BLENDING FORMULA:

destCol=((A-B)*C+D)>>7

where

A,B,D	= srcCol or destCol or 0
C		= srcAlpha or destAlpha or 0

*/

#elif(BPLATFORM == GAMECUBE)

#error "PP: Hi, could we have [destCol+(srcCol*srcAlpha)] blending here please - I don't know how to do it myself.  Cheers."
#error "PP: Give's a shout if this isn't possible and I'll try something else.  LIKE A REVOLVER."

#else// PP: unsupported platform
#error "PP: Hi, could we have [destCol+(srcCol*srcAlpha)] blending here please - I don't know how to do it myself.  Cheers."
#error "PP: Give's a shout if this isn't possible and I'll try something else.  LIKE A REVOLVER."
#endif// PP: platform

#endif// PP: ndef AW_JINGS_IT_WORKS_WITH_NORMAL_ADDITIVE_AFTER_ALL

		// PP: bugger
		SETBLEND(ADDITIVE);

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,&VFX_shadowBlendBlurVerts,SHADOWMAP_NUM_BLUR_VERTS,BVERTTYPE_SINGLE);
	}
	else// PP: subtractive
	{
		ENABLEBLEND(TRUE);
		// PP: trust me!
		SETBLEND(ADDITIVE);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,&VFX_shadowSubtractiveBlurVerts,SHADOWMAP_NUM_BLUR_VERTS,BVERTTYPE_SINGLE);
	}
}
#endif// PP: def BLUR_SHADOWMAPS


/*	--------------------------------------------------------------------------------
	Function 	: VFX_generateShadowMap
	Purpose 	: Create a shadow map of a character
	Parameters 	: ptr ACTORINSTANCE whose shadow map to generate
	Returns 	: 
	Info 		: // PP: TODO: optimise the shadow system
*/

// PP: twico
float	testDepth=0.999999f;
float	testBias=0.0f;

// PP: distance of camera from occluder when rendering a silhouette from a directional light
#define VFX_SHADOWMAP_INFINITE_DISTANCE		METERTOUNIT(35)

// PP: z-bias used when drawing the shadowmap's background sprite
// PP: We'd just have to find a way of doing the background in 2d properly
#if(BPLATFORM == PS2)
#define VFX_SHADOWMAP_BACKGROUND_ZBIAS		4000.0f
#else// PP: !ps2							// PP: gotta love those z scales
#define VFX_SHADOWMAP_BACKGROUND_ZBIAS		0// PP: 0.002f
#endif// PP: !ps2

// PP: size of the shadowmap's background sprite
#define VFX_SHADOWMAP_BACKGROUND_SIZE		300

void VFX_generateShadowMap(ACTORINSTANCE* const actorInstance)
{
	int		flags;

	if(actorInstance->flags & ACTORFLAG_DONTDRAW) return;
	if(!actorInstance->currentAlpha) return;

	// PP: FIRST THINGS FIRST -
	actorInstance->actorInstance.position[W]=1.0f;

	// PP: a'ight get a ptr to the shadow light that gonna do this thang
	SHADOWLIGHT*	light;

	light=actorInstance->actorShadow->light;

	// PP: select appropriate set of flags from the light
	flags=light->getShadowFlags(actorInstance);

	// PP: set the VFX_renderTarget as the current render target

	COLOUR	clearCol;
	
	if(light->blend == SHADOWBLEND_BLEND)
	{
		// PP: clear ORT to 0 alpha
		clearCol=COLOUR(0,255,0,255);//255
	}
	else// PP: subtractive
	{
		// PP: clear ORT to full rgb
		clearCol=COLOUR(255,255,255,255);
	}

	pushRenderTarget(
#ifdef BLUR_SHADOWMAPS
		VFX_tempShadowRenderTarget,
#else// PP: no blur
		VFX_shadowRenderTarget,
#endif// PP: no blur
		clearCol.r, clearCol.g, clearCol.b, clearCol.a, 1.0f, BSETRENDERTARGET_CLEAR);

	// PP: only write to the zbuffer
	SETZWRITE(TRUE);

	// PP: but not the framebuffer
	DISABLE_FRAMEWRITE;

	SETPROJECTION(3D)
	SETZTEST(LESSTHAN);

	// PP: TURN OFF FOG!!!
	bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, 0);

	// PP: SET UP CAMERA...

	TBVector	camPos;
	float		dist;

	if(light->type == SHADOWLIGHTTYPE_POINT)
	{
		bmVectorCopy(camPos, light->pos.v);

		// PP: calculate & store FOV to use when rendering shadow map...

		// PP: store dist in shadow structure

		VEC		distVec;

		distVec=actorInstance->actorInstance.position-light->pos;

		dist=actorInstance->actorShadow->shadowMapDist=distVec.magnitude();

		if(dist != 0.0f)
		{
			light->dir=distVec;

			if(flags & SHADOWLIGHTFLAG_ANGULAR_FALLOFF)
			{
				// PP: we're gonna need that dir vector normalised for the angular falloff dot-product
				light->dir.normalise2();
			}

			light->ang=distVec.ang();
		}
	}
	else if(light->type == SHADOWLIGHTTYPE_DIRECTIONAL)
	{
		dist=VFX_SHADOWMAP_INFINITE_DISTANCE;

		if(light->range != SHADOWLIGHT_INFINITE_RANGE)// PP: what are you up to - a distance-ranged directional light?  I hope you know what you're doing, you wacky artist type!
		{
			actorInstance->actorShadow->shadowMapDist=bmVectorDistance(light->pos.v, actorInstance->actorInstance.position);
		}

		bmVectorCopy(camPos, -light->dir*VFX_SHADOWMAP_INFINITE_DISTANCE);
		bmVectorAdd(camPos, camPos, actorInstance->actorInstance.position);
	}

	if(dist != 0.0f)// PP: prevent db0
	{
		if(dist<=actorInstance->actorShadow->shadowMapRadius)// PP: values greater than 1 are invalid input for ASin: max allowable fov is therefore 90°
		{
			actorInstance->actorShadow->shadowMapFOV=HALFPI;

		//	bkPrintf("!!!!!! fov %f\n", actorInstance->actorShadow->shadowMapFOV);
		}
		else
		{
			// PP: calculate & store FOV to use when rendering shadow map...
			actorInstance->actorShadow->shadowMapFOV=bmASin(actorInstance->actorShadow->shadowMapRadius/dist);

		//	bkPrintf("fov %f\n", actorInstance->actorShadow->shadowMapFOV);
		}
	}

	// PP: store the angle of the light relative to this character
	actorInstance->actorShadow->lightAngle=light->ang;

	bdSetFOV(actorInstance->actorShadow->shadowMapFOV, actorInstance->actorShadow->shadowMapFOV);

	bdSetViewPosition(camPos[X], camPos[Y], camPos[Z]);

	bdSetViewLookAt(camPos, actorInstance->actorInstance.position, 0.0f);

	bdSetupView();

// PP: ~~~~~~~~

	EBActRenderMode		renderModeCopies[VFX_SHADOWMAP_NUM_RENDERMODE_STAGES];
	void*				renderParmCopies[VFX_SHADOWMAP_NUM_RENDERMODE_STAGES];

	// PP: copy actor instance's render modes for safekeeping

	int stage;

	for(stage=0;stage<VFX_SHADOWMAP_NUM_RENDERMODE_STAGES;stage++)
	{
		renderModeCopies[stage]=actorInstance->actorInstance.renderMode[stage];
		renderParmCopies[stage]=actorInstance->actorInstance.renderParms[stage];
	}

	baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);// PP: // PP: NOTE: Z-only mode doesn't take into account colour-keying D'OH!
	baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_NONE, NULL);

	// PP: draw actor instance...

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetIdentityObjectMatrix();// PP: new
	baSetGlobalAlpha(actorInstance->currentAlpha);

#if BPLATFORM == PC
	bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif

	baDrawActorInstance(&actorInstance->actorInstance);

	// PP: restore actor instance's old render modes
	for(stage=0;stage<VFX_SHADOWMAP_NUM_RENDERMODE_STAGES;stage++)
	{
		actorInstance->actorInstance.renderMode[stage]=renderModeCopies[stage];
		actorInstance->actorInstance.renderParms[stage]=renderParmCopies[stage];
	}

	// PP: draw attached objects...

	if(actorInstance->characterInfo != NULL)
	{
	//	PrepAttachedObjects(actorInstance);
	//	PrepCharactersHealth(actorInstance);
	//#ifdef NOTYET
		int		i;

		for(i=0;actorInstance->characterInfo->attachedObjects.objects[i]!=NULL;i++)
		{
			// PP: copy actor instance's render modes for safekeeping
			for(stage=0;stage<VFX_SHADOWMAP_NUM_RENDERMODE_STAGES;stage++)
			{
				renderModeCopies[stage]=actorInstance->characterInfo->attachedObjects.objects[i]->renderMode[stage];
				renderParmCopies[stage]=actorInstance->characterInfo->attachedObjects.objects[i]->renderParms[stage];
			}

			baSetActorRenderMode(actorInstance->characterInfo->attachedObjects.objects[i], 0, BACTRENDERMODE_NORMAL, NULL);// PP: NOTE: Z-only mode doesn't take into account colour-keying D'OH!
			baSetActorRenderMode(actorInstance->characterInfo->attachedObjects.objects[i], 1, BACTRENDERMODE_NONE, NULL);
			//baSetActorRenderMode(actorInstance->characterInfo->attachedObjects.objects[i], 1, BACTRENDERMODE_OUTLINED, &VFX_shadowLineParms);

			baSetGlobalAlpha(actorInstance->currentAlpha);

			baDrawActorInstance(actorInstance->characterInfo->attachedObjects.objects[i],BACTPRIMALPHATYPE_ALL);

			// PP: BOUNDS TEST
			//utilDrawCircle(actorInstance->actorInstance.position, actorInstance->actorShadow->shadowMapRadius, COLOUR(255,255,255));

			// PP: restore actor instance's old render modes
			for(stage=0;stage<VFX_SHADOWMAP_NUM_RENDERMODE_STAGES;stage++)
			{
				actorInstance->characterInfo->attachedObjects.objects[i]->renderMode[stage]=renderModeCopies[stage];
				actorInstance->characterInfo->attachedObjects.objects[i]->renderParms[stage]=renderParmCopies[stage];
			}
		}
	}

	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
//	bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
//#endif// PP: def NOTYET

	// PP: draw a black rectangle behind the character

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);

	// PP: write to frame buffer again
	REENABLE_FRAMEWRITE;

	bdSetIdentityObjectMatrix();

/*	bdSetZBias(0);
	SETPROJECTION(3D);
*/
	//TBMatrix	testMat;

	//RECTANGLE	testRect;

	//static float d=METERTOUNIT(10000);

	
	// PP: NEW TEST
	NOTEXTURE;

	//testRect=RECTANGLE(-d, d, -d, d);

	// PP: NEW TEST: drawing at depth of almost 1.0f, rather than fiddling with zbias

	if(light->blend == SHADOWBLEND_BLEND)
	{
		// PP: enable alpha framewrite
		SETFRAMEWRITE(rgbFramewriteChannels|BDFRAMEWRITE_ALPHA);

		// PP: must disable blending because blending would interfere with alpha writing, y'know?
		ENABLEBLEND(TRUE);// PP: false

		// PP: draw background with alpha almost 0
		//testRect.draw(COLOUR(128,128,128,1), SOLIDFILL, X, Y, testDepth);
	
		// PP: NOTE: This sprite is getting needlessly transformed from 3d into 2d.  OH BOO HOO!!
		// PP: But just *you* try getting 2d verts to the back of the zbuffer on Ps2.  I dare ya!
		// PP: NOTE: it seems I need at least one bit of alpha in order for the poly to draw at all
		bdDrawFixedFlatSprite(actorInstance->actorInstance.position, VFX_SHADOWMAP_BACKGROUND_SIZE, VFX_SHADOWMAP_BACKGROUND_SIZE, 0.0f, NULL, 0, 0, 255, 0, 0, 30, NULL, VFX_SHADOWMAP_BACKGROUND_ZBIAS);

		// PP: re-enable alpha
		ENABLEBLEND(TRUE);
	}
	else// PP: subtractive
	{
		// PP: draw background with rgb 0

		// PP: NOTE: This sprite is getting needlessly transformed from 3d into 2d.  OH BOO HOO!!
		// PP: But just *you* try getting 2d verts to the back of the zbuffer on Ps2.  I dare ya!
		if(bdDrawFixedFlatSprite(actorInstance->actorInstance.position, VFX_SHADOWMAP_BACKGROUND_SIZE, VFX_SHADOWMAP_BACKGROUND_SIZE, 0.0f, NULL, 0, 0, 0, 0, 0, 255, NULL, VFX_SHADOWMAP_BACKGROUND_ZBIAS)
			!= OK)
		{
			bkPrintf("sprite out of sight\n");
		}
	}

#ifdef BLUR_SHADOWMAPS
	VFX_blurShadowMap(light);
#endif// PP: def BLUR_SHADOWMAPS

	// PP: disable alpha framewrite again
	SETFRAMEWRITE(rgbFramewriteChannels);

	// PP: SET CAMERA BACK TO HOW IT WAS
	SendCamera();
	bdSetFOV(CURRENT_SCREEN.horzFov, CURRENT_SCREEN.vertFov);
	bdSetupView();

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	// switch back to the primary target
	popRenderTarget();

	// PP: NEW TEST - set viewport to what it should be
	bdSetViewport(int(videoMode.screens[gameStatus.multiplayer.currentScreen].viewport.left), int(videoMode.screens[gameStatus.multiplayer.currentScreen].viewport.top), int(videoMode.screens[gameStatus.multiplayer.currentScreen].viewport.widthAbs()), int(videoMode.screens[gameStatus.multiplayer.currentScreen].viewport.heightAbs()));

	// PP: NEW TESTS!!!
	NOTEXTURE;
	WRAPTEXTURE;

	// PP: return fogging to correct state
	RESTOREFOG;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_shadowMapPostDrawDebug
	Purpose 	: display the final shadow map
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_shadowMapPostDrawDebug(void)
{

	// PP: BOUNDS TEST
//	utilDrawCircle(gameStatus.player1->actorInstance.position, gameStatus.player1->actorShadow->shadowMapRadius, COLOUR(0,255,0));

	if(VFX_shadowRenderTarget == NULL)
	{
		return;
	}

//	bdBeginScene();// PP: PAH?

	// PP: draw the render target to the screen at appropriate alpha
	
	bdSetIdentityObjectMatrix();

	RECTANGLE	destRect;
	
	destRect=(VFX_DRAWRENDERTARGET_VIEWPORT_RECT*0.85f).leftHalf().topHalf().leftHalf().topHalf();

	VFX_drawRenderTarget(COLOUR(128, 128, 128, 128),
							BDALPHABLENDMODE_BLEND,
							0,
							NULL,
							&destRect,
							VFX_shadowRenderTarget,
							&VFX_shadowRenderTargetInfo,
							VFX_SHADOW_RENDER_TARGET_WIDTH,
							VFX_SHADOW_RENDER_TARGET_HEIGHT);

	destRect.draw(GREEN, OUTLINE);

#ifdef BLUR_SHADOWMAPS

	destRect=(VFX_DRAWRENDERTARGET_VIEWPORT_RECT*0.85f).leftHalf().topHalf().leftHalf().bottomHalf();

	VFX_drawRenderTarget(COLOUR(128, 128, 128, 128),
							BDALPHABLENDMODE_BLEND,
							0,
							NULL,
							&destRect,
							VFX_tempShadowRenderTarget,
							&VFX_shadowRenderTargetInfo,
							VFX_SHADOW_RENDER_TARGET_WIDTH,
							VFX_SHADOW_RENDER_TARGET_HEIGHT);

	destRect.draw(RED, OUTLINE);

#endif// PP: def BLUR_SHADOWMAPS

//	bdEndScene();// PP: PAH?
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													SCREEN SPIN-OFF

		Takes a snapshot of the scene, and quickly spins it off of the screen to reveal the next shot underneath.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define VFX_SPINOFF_DURATION					1.3f						// PP: duration of a spin-off effect
#define VFX_SPINOFF_MAXIMIZED_SCALE				1.0f						// PP: initial scale of the spin-off effect
#define VFX_SPINOFF_MINIMIZED_SCALE				0.0f						// PP: final scale of the spin-off effect
#define VFX_SPINOFF_MAXIMIZED_X					0.0f						// PP: initial x pos of effect
#define VFX_SPINOFF_MAXIMIZED_Y					0.0f						// PP: initial y pos of effect
#define VFX_SPINOFF_MINIMIZED_X					(CURRENT_VIEWPORT.widthAbs()/2.0f)		// PP: final x pos of effect
#define VFX_SPINOFF_MINIMIZED_Y					(CURRENT_VIEWPORT.widthAbs()/2.0f)	// PP: final y pos of effect
#define VFX_SPINOFF_X_TURNS						0.30f						// PP: number of X turns in the spin-off effect
#define VFX_SPINOFF_Y_TURNS						0.0f						// PP: number of Y turns in the spin-off effect
#define VFX_SPINOFF_Z_TURNS						-2.1f						// PP: number of Z turns in the spin-off effect
#define VFX_SPINOFF_DRAWDEPTH					0.5f						// PP: draw depth of effect
#define VFX_SPINOFF_NUM_BLUR_FRAMES				3							// PP: number of trailing motion-blur frames (this doesn't include the current frame)
#define VFX_SPINOFF_SHADOW_OFFSET				40							// PP: shadow offset

static float												VFX_spinoffProgress;		// PP: progress (0..1) of spinoff effect
static LOOPARRAY<TBMatrix, VFX_SPINOFF_NUM_BLUR_FRAMES+1>	VFX_spinoffObjectMatrices;	// PP: object matrix used when drawing the VFX_renderTarget
static float												VFX_spinoffScale;

#define VFX_SPINOFF_DEBUG

// PP: most of these chaps get updated from the game loops (main.cpp)...

bool				VFX_spinoffPrePreparing;		// PP: spinoff will get prepared over the next frame
bool				VFX_spinoffPreparing;			// PP: spinoff is being prepared over this frame
bool				VFX_spinoffNewlyPrepared;		// PP: spinoff is newly prepared and ready to start
bool				VFX_spinoffPrepared;			// PP: spinoff is prepared and ready to start
bool				VFX_spinoffNewlyStarted;		// PP: spinoff has just started - could check for this before changing camera shots
bool				VFX_spinoffStarted;				// PP: spinoff has started - could check for this before changing camera shots


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPreparePreDraw
	Purpose 	: one-time pre-draw for spinoff
	Parameters 	: 
	Returns 	: 
	Info 		: sets the VFX_renderTarget as the current render target
*/
void VFX_spinoffPreparePreDraw(void)
{
#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffPreparePreDraw\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	// PP: use the VFX_renderTarget for this effect
	VFX_useRenderTarget(VFX_SPINOFF);

	// PP: set the VFX_renderTarget as the current render target
	pushRenderTarget(VFX_renderTarget, 0,0,0, 255, 1.0f, BSETRENDERTARGET_CLEAR);

	// PP: override stereoscopy to bypass a world of hurt
	//overrideStereoscopy(true);
/*	rgbFramewriteChannels=BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE;// PP: bodge
	extern bool g_overrideStereoscopy;
	g_overrideStereoscopy=true;*/

	// PP: spinoff is being prepared over this frame
	VFX_spinoffPrePreparing=false;
	VFX_spinoffPreparing=true;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPrepare
	Purpose 	: prepare a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This could be called one frame before a change of camera shot
					Sets the VFX_renderTarget as the current render target
*/
void VFX_spinoffPrepare(void)
{
	// PP: MIlESTONEFRIG FOR ZOOBOSS
	if(VFX_spinoffPreparing)
	{
		return;
	}

#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffPrepare\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	VFX_activateEffect(VFX_SPINOFF);

	VFX_spinoffProgress=0.0f;

	VFX_spinoffObjectMatrices.init();

	// PP: spinoff isn't prepared until the fat bloke sings, at the end of this game loop that has just started...

	VFX_spinoffPrePreparing=true;
	VFX_spinoffPreparing=false;
	VFX_spinoffPrepared=false;
	VFX_spinoffNewlyPrepared=false;
	VFX_spinoffStarted=false;
	VFX_spinoffNewlyStarted=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffStart
	Purpose 	: start a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This can be called at any time once VFX_spinoffPrepared is true
*/
void VFX_spinoffStart(void)
{
#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffStart\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	VFX_spinoffStarted=true;
	VFX_spinoffNewlyStarted=true;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffUpdate
	Purpose 	: update a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This gets called by VFX_update while VFX_effectActive(VFX_SPINOFF)
*/
void VFX_spinoffUpdate(void)
{
#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffUpdate\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	TBVector		rot;
	TBVector		pos;

	if(VFX_spinoffStarted)
	{
		// PP: update progress
		VFX_spinoffProgress += (fTime/VFX_SPINOFF_DURATION);

		if(VFX_spinoffProgress > 1.0f)
		{
			VFX_deactivateEffect(VFX_SPINOFF);
			return;
		}
	}

	TBMatrix		rotMat;						
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;
	TBMatrix		tempMat2;

	TBMatrix*		objectMatrix;

	objectMatrix=VFX_spinoffObjectMatrices.getNew();

	// PP: update scale
	VFX_spinoffScale=VFX_SPINOFF_MAXIMIZED_SCALE-(VFX_spinoffProgress*(VFX_SPINOFF_MAXIMIZED_SCALE-VFX_SPINOFF_MINIMIZED_SCALE));

	// PP: update rotation...

	// PP: MILESTONE FRIG
	// PP: REMOUT: seems to be 'working' again now		static float zRotFrig=PI;

	SETVECTOR(rot, VFX_spinoffProgress*VFX_SPINOFF_X_TURNS*TWOPI, VFX_spinoffProgress*VFX_SPINOFF_Y_TURNS*TWOPI, /*zRotFrig+*/VFX_spinoffProgress*VFX_SPINOFF_Z_TURNS*TWOPI, 1.0f);

	// PP: udpate position
	SETVECTOR(pos, VFX_SPINOFF_MAXIMIZED_X-(VFX_spinoffProgress*(VFX_SPINOFF_MAXIMIZED_X-VFX_SPINOFF_MINIMIZED_X)),
				VFX_SPINOFF_MAXIMIZED_Y-(VFX_spinoffProgress*(VFX_SPINOFF_MAXIMIZED_Y-VFX_SPINOFF_MINIMIZED_Y)),
				VFX_SPINOFF_DRAWDEPTH,
				1.0f);
								
	bmMatScale(scaleMat, VFX_spinoffScale, VFX_spinoffScale, VFX_spinoffScale);
	bmMatXZYRotation(rotMat, rot[X], rot[Y], rot[Z]);
	bmMatTranslate(posMat, pos[X], pos[Y], pos[Z]);

	bmMatMultiply(tempMat, scaleMat, rotMat);
	bmMatMultiply(tempMat2, posMat, tempMat);

	// PP: squish everything on the Z
	bmMatScale(scaleMat, 1.0f, 1.0f, 0.0f);

	bmMatMultiply(*objectMatrix, scaleMat, tempMat2);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPostPrepare
	Purpose 	: finish preparing a spinoff
	Parameters 	: 
	Returns 	: 
	Info 		: This is called from game loops (main.cpp)
*/
void VFX_spinoffPostPrepare(void)
{
#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffPostPrepare\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	VFX_spinoffPreparing=false;
	VFX_spinoffNewlyPrepared=true;
	VFX_spinoffPrepared=true;

	// PP: finish overriding stereoscopy
	//overrideStereoscopy(false);
	extern bool g_overrideStereoscopy;
	g_overrideStereoscopy=false;

	// switch back to the primary target, note we need to clear the Z buffer as our shared target will have modified it
	popRenderTarget(0,0,0,255, 1.0f, BSETRENDERTARGET_CLEARZ);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPostDraw
	Purpose 	: draw a spin-off effect in progress
	Parameters 	: 
	Returns 	: 
	Info 		: This is called from game loops (main.cpp) while VFX_effectActive(VFX_SPINOFF)
*/
void VFX_spinoffPostDraw(void)
{
#ifdef VFX_SPINOFF_DEBUG
	bkPrintf("VFX_spinoffPostDraw\n");
#endif// PP: def VFX_SPINOFF_DEBUG

	// PP: this is safe in a post-draw function
	if(VFX_spinoffStarted)
	{
		VFX_spinoffNewlyStarted=false;
	}

	COLOUR			colour;
	COLOUR			shadowColour=COLOUR(80,80,80,70);// PP: ?

	int				i;

	TBMatrix		shadowMatrix;					// PP: object matrix to use for the drop-shadow

	TBPrimVertex	shadowVerts[4];

	bdSetTexture(0,NULL);

	// PP: WARNING (efficiency): PRETTY OBVIOUS!
	
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 1);

	float	ratio;

	colour.a=shadowColour.a;

	i=1;

	LA_FOREACH(VFX_spinoffObjectMatrices, TBMatrix, objectMatrix)

		bmMatCopy(shadowMatrix, *objectMatrix);

		shadowMatrix[3][1]-=VFX_SPINOFF_SHADOW_OFFSET*VFX_spinoffScale;
		shadowMatrix[3][0]+=VFX_SPINOFF_SHADOW_OFFSET*VFX_spinoffScale;

		bdSetObjectMatrix(shadowMatrix);

		ratio=i/float(VFX_spinoffObjectMatrices.size);

		colour.r=(unsigned char)(shadowColour.r*ratio);
		colour.g=(unsigned char)(shadowColour.g*ratio);
		colour.b=(unsigned char)(shadowColour.b*ratio);

		for(int v=0; v<4; v++)
		{
			BDVERTEX_SETCOLOUR(&shadowVerts[v], colour);
			BDVERTEX_SETXYZW(&shadowVerts[v], ((v&1)?(-0.5f):(0.5f))*CURRENT_VIEWPORT.width(), ((v/2)?(-0.5f):(0.5f))*CURRENT_VIEWPORT.height(), 0.0f, 0.0f);
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,shadowVerts,4,BVERTTYPE_SINGLE);

		i++;

	LA_ENDFOREACH(VFX_spinoffObjectMatrices);

	// PP: ~~~~~

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	colour=COLOUR(128,128,128);

	i=1;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	LA_FOREACH(VFX_spinoffObjectMatrices, TBMatrix, objectMatrix)
		
		bdSetObjectMatrix(*objectMatrix);

		colour.a=(unsigned char)(128*(i/float(VFX_spinoffObjectMatrices.size)));

		VFX_drawRenderTarget(	colour,
								BDALPHABLENDMODE_BLEND,
								BSETRENDERTARGET_CLEARZ,
								NULL,
								NULL,
								VFX_renderTarget,
								&VFX_renderTargetInfo,
								VFX_RENDER_TARGET_WIDTH,
								VFX_RENDER_TARGET_HEIGHT,
								false);
	
		i++;

	LA_ENDFOREACH(VFX_spinoffObjectMatrices);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

	bdSetIdentityObjectMatrix();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TIMESLICE

									Freezes time and rotates the camera around a point.

										'Flow-Motion™" or 'Matrix-Cam™' if you insist.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: define TIMESLICE_360 to use a 360° timeslice instead of 90°
#define TIMESLICE_360

#ifdef TIMESLICE_360

#define VFX_TIMESLICE_NUM_CAMERAS			96			// PP: number of cameras on the timeslice rig
#define VFX_TIMESLICE_ANGLE_RANGE			TWOPI		// PP: angle range of the arc
#define VFX_TIMESLICE_DURATION				3.0f		// PP: time taken to arc around the focus point

#else// PP: ndef TIMESLICE_360

// PP: 90°

#define VFX_TIMESLICE_NUM_CAMERAS			24			// PP: number of cameras on the timeslice rig
#define VFX_TIMESLICE_ANGLE_RANGE			HALFPI		// PP: angle range of the arc
#define VFX_TIMESLICE_DURATION				1.5f		// PP: time taken to arc around the focus point

#endif// PP: ndef TIMESLICE_360

#define VFX_TIMESLICE_MAX_LOOKAT_OFFSET		3.0f		// PP: maximum lookat offset of each camera

#if(BPLATFORM == PS2)// PP: hmm, well, y'know...
#define VFX_TIMESLICE_MOTION_BLUR_LEVEL		0.4f		// PP: motion blur level for VFX_TIMESLICE
#else// PP: !ps2
#define VFX_TIMESLICE_MOTION_BLUR_LEVEL		0.2f		// PP: motion blur level for VFX_TIMESLICE
#endif// PP: !ps2
														
#if(BPLATFORM == PS2)									
#define VFX_TIMESLICE_MAX_TINT				22			// PP: maximum tint (+/-) of each camera
#define VFX_TIMESLICE_BASE_TINT				8			// PP: base tint (+) of all cameras
#else // PP: if(BPLATFORM == PS2) else					
#define VFX_TIMESLICE_MAX_TINT				22//7			// PP: maximum tint (+/-) of each camera
#define VFX_TIMESLICE_BASE_TINT				8//2			// PP: base tint (+) of all cameras
#endif// PP: (BPLATFORM == PS2) else

#define VFX_TIMESLICE_INITIAL_HEIGHT		1.5f		// PP: minimum camera offset from lookat height (metres)
#define VFX_TIMESLICE_FINAL_HEIGHT			(-0.5f)		// PP: minimum camera offset from lookat height (metres)
#define VFX_TIMESLICE_HEIGHT_RANGE			(VFX_TIMESLICE_FINAL_HEIGHT-VFX_TIMESLICE_INITIAL_HEIGHT)// PP: yadda height range
													
#define VFX_TIMESLICE_TINT_ALPHA			64			// PP: tint alpha - might need calibration so's PS2 & PC match

static TBVector		VFX_timesliceFocus;					// PP: focus point of the cameras
static float		VFX_timesliceProgress;				// PP: progress (0..1) of the timeslice effect
static float		VFX_timesliceLastProgress;			// PP: last update's progress (0..1)
static VEC			VFX_timesliceLookatCopy;			// PP: copy of camera's lookat point										
static int			VFX_timesliceCamIndex;				// PP: index of virtual camera on the virtual rig
static bool			VFX_timesliceTintInitialised;		// PP: tint is initialised
static int			VFX_timesliceDirection;				// PP: direction of rotation
														
float				VFX_timesliceTimer=0.0f;			// PP: timer you can use to delay triggering timeslice
bool				VFX_timesliceLatch=false;			// PP: latch you can use to control triggering timeslice


/*	--------------------------------------------------------------------------------
	Function 	: VFX_timeslicePauseUpdateFunc
	Purpose 	: pause update function for the timeslice effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_timeslicePauseUpdateFunc(const int32 parameter)
{
	VFX_timesliceProgress+=(fTime/VFX_TIMESLICE_DURATION);

	// PP: whack some motion blur on...
	VFX_motionBlur(VFX_TIMESLICE_MOTION_BLUR_LEVEL);

	// PP: CHECK FOR CROSSING A CAMERA BOUNDARY...

	if(int(VFX_timesliceProgress*VFX_TIMESLICE_NUM_CAMERAS) != int(VFX_timesliceLastProgress*VFX_TIMESLICE_NUM_CAMERAS))
	{	
		VEC			camPos;
		float		yAngle;
		float		height;

		// PP: crossed a camera boundary
		VFX_timesliceCamIndex++;

		// PP: update angle...
		
		yAngle= VFX_timesliceCamIndex*VFX_timesliceDirection*(VFX_TIMESLICE_ANGLE_RANGE/VFX_TIMESLICE_NUM_CAMERAS);

		// PP: NEW
		height=VFX_TIMESLICE_INITIAL_HEIGHT+(VFX_timesliceProgress*VFX_TIMESLICE_HEIGHT_RANGE);
		camPos=VFX_timesliceLookatCopy+(VECM(2.0f, height, 0).rotateY(yAngle));

		bkPrintf("height %5.2f\n", height);
		bkPrintf("yAngle %5.2f\n", yAngle);

		// PP: set camera position
		SetCameraPosition(VFX_timesliceCam, camPos);

		// PP: OOPS! PRECISION ENGINEERING JUST AIN'T WHAT IT USED TO BE!
		// PP: add a slight offset to the lookat...

		VEC			offset;

		SETVECTOR(offset, (-1.0f+(bmRandf()*2.0f))*VFX_TIMESLICE_MAX_LOOKAT_OFFSET,
							(-1.0f+(bmRandf()*2.0f))*VFX_TIMESLICE_MAX_LOOKAT_OFFSET,
							(-1.0f+(bmRandf()*2.0f))*VFX_TIMESLICE_MAX_LOOKAT_OFFSET,
							0.0f);

		SetCameraLookAtPosition(VFX_timesliceCam, VFX_timesliceLookatCopy+offset);
	}

	// PP: END THE EFFECT IF IT HAS FINISHED...
		
	if(VFX_timesliceProgress > 1.0f)
	{
		// PP: effect finished
		VFX_deactivateEffect(VFX_TIMESLICE);
	}

	VFX_timesliceLastProgress=VFX_timesliceProgress;
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_timesliceDraw
	Purpose 	: draw for the timeslice effect
	Parameters 	: 
	Returns 	: 
	Info 		: applies lightness errors	
*/
void VFX_timesliceDraw(void)
{
	static int			lastCamIndex;
	static COLOUR		col;
	static int			blendMode;
	TBPrimVertex		verts[4];
	static int			tint;

	if(VFX_timesliceCamIndex==0)
	{
		// PP: NEW: flash of white to start with
		blendMode=BDALPHABLENDMODE_ADDITIVE;

		col=COLOUR(240,240,240,240);
	}
	else
	{
		if((!VFX_timesliceTintInitialised) || (VFX_timesliceCamIndex != lastCamIndex))
		{
			// PP: OOPS! SHOULD'VE CLEANED THOSE LENSES!
			// PP: each camera has a slightly different lightness

			tint=bmFloatToInt(VFX_TIMESLICE_BASE_TINT+(bmRandfSigned()*VFX_TIMESLICE_MAX_TINT));

			if(tint>0)
			{
				// PP: additive tint
				blendMode=BDALPHABLENDMODE_ADDITIVE;
			}
			else if(tint<0)
			{
				// PP: subtractive tint
				blendMode=BDALPHABLENDMODE_SUBTRACTIVE;

				tint=(-tint);
			}

			col=COLOUR(tint, tint, tint, VFX_TIMESLICE_TINT_ALPHA);

		}
	}

	VFX_timesliceTintInitialised=true;

	lastCamIndex=VFX_timesliceCamIndex;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();
										 
	#if BPLATFORM == PS2
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	#endif	

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, blendMode, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, blendMode, 1);
	bdSetTexture(0,NULL);

	for(int v=0; v<4; v++)
	{
		BDVERTEX_SETCOLOUR(&verts[v], col);
		BDVERTEX_SETXYZW(&verts[v], ((v&1)?(-1.0f):(1.0f))*GLARE_RECT_WIDTH, ((v/2)?(-1.0f):(1.0f))*GLARE_RECT_HEIGHT, 0.0f, 0.0f);
	}

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_timeslice
	Purpose 	: start a timeslice
	Parameters 	: 
	Returns 	: 
	Info 		: uses camera[0], starts from its current position and rotates 90° around its focus point
					Might make this a bit more customisable at some point
					
*/
void VFX_timeslice()
{
	// PP: reset timers
	VFX_timesliceProgress=0.0f;
	VFX_timesliceLastProgress=0.0f;

	// PP: record camera info
	VFX_timesliceCam= &camera[0];

	// PP: TEST - again
	bmVectorCopy(VFX_timesliceCam->lookAt, VFX_timesliceCam->destinationLookAt);

	bmVectorCopy(VFX_timesliceLookatCopy, VFX_timesliceCam->lookAt);
	bmVectorCopy(VFX_timesliceCamPosCopy, VFX_timesliceCam->pos);


	// PP: lalala
	VFX_timesliceCamIndex=0;
	VFX_timesliceTintInitialised=false;
	VFX_timesliceLatch=true;
	VFX_timesliceDirection=(bmRand()&1)?1:(-1);

	// PP: freeze time
	SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, int32(VFX_timeslicePauseUpdateFunc), NULL);
	
	// PP: set the effect active
	VFX_activateEffect(VFX_TIMESLICE);

	// PP: for that tacky "cinematic" thing
	setFilmBorders(true);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															FOCUS

								Camera focus effects, courtesy of the lovely Mr. Sidwell
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: TODO: ask Sidwell if bi-directional DOF is feasible

// PP: ---------

#define VFX_FOCUS_DEFAULT_BLUR_FACTOR		1			// PP: default number of blur passes on the defocused frame

// PP: focus modes
enum VFXFocusMode
{
	VFXFOCUS_NEAR,
	VFXFOCUS_FAR,
	VFXFOCUS_BLUR
};

static bool					VFX_focusPrepared=false;	// PP: is the display prep'd for focus effects?
static ACTORINSTANCE*		VFX_focusObject=NULL;		// PP: ptr to object to be kept in focus
static VFXFocusMode			VFX_focusMode;				// PP: focus mode (near/far)
static float				VFX_focusLength;			// PP: focal length to use for the focus effects
static CAMERAINFO*			VFX_focusCamera;			// PP: ptr to camera used for focus tracking (&camera[0])
static float				VFX_focusNearVal;			// PP: near focus value
static float				VFX_focusFarVal;			// PP: far focus value
static int					VFX_focusBlurFactor;		// PP: number of blur passes on the defocused frame
static int					VFX_focusNextBlurFactor;	// PP: number of blur passes on the defocused frame


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initFocus
	Purpose 	: one-time init for focus effects, called by VFX_init
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initFocus(void)
{

#if(BPLATFORM==PS2)

	VFX_focusCamera= &camera[0];

	VFX_focusSetBlurFactor(VFX_FOCUS_DEFAULT_BLUR_FACTOR);

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPrepare
	Purpose 	: Prepare the display for focus effects
	Parameters 	: 
	Returns 	: true if the preparation was successful, false otherwise
	Info 		: puts the display into 32 bit
*/
bool VFX_focusPrepare(void)
{
	if(VFX_focusPrepared) return true;

#if(BPLATFORM==PS2)

	videoMode.bppScreen=32;
	videoMode.xScreen=XSCREEN_32BIT;
	videoMode.yScreen=YSCREEN_32BIT;

	bdCloseDisplay();

	if (bdOpenDisplay(videoMode.xScreen,videoMode.yScreen, 
		videoMode.bppScreen, videoMode.zScreen, videoMode.flags))
	{
		VFX_focusPrepared=true;

		// PP: need to do this to change viewport sizes
		setNumScreens(gameStatus.multiplayer.numScreens);

		return true;
	}
	else
	{
		bkAlert("VFX_focusPrepare: Could not re-open display in 32 bit.");

		return false;
	}
#else// PP: if(BPLATFORM==PS2) else

	// PP: TEMP?: can't do focus effects on this platform at all :'o(
	return false;

#endif// PP: (BPLATFORM==PS2)
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusUnprepare
	Purpose 	: Reset the display after focus effects
	Parameters 	: 
	Returns 	: true if the reset was successful, false otherwise
	Info 		: puts the display back into 16 bit
*/
bool VFX_focusUnprepare(void)
{
	if(!VFX_focusPrepared) return true;

#if(BPLATFORM==PS2)

	videoMode.bppScreen=16;
	videoMode.xScreen=XSCREEN_16BIT;
	videoMode.yScreen=YSCREEN_16BIT;

	bdCloseDisplay();

	if (bdOpenDisplay(videoMode.xScreen,videoMode.yScreen, 
		videoMode.bppScreen, videoMode.zScreen, videoMode.flags))
	{
		VFX_focusPrepared=false;

		// PP: need to do this to change viewport sizes
		setNumScreens(gameStatus.multiplayer.numScreens);

		return true;
	}
	else
	{
		bkAlert("VFX_focusUnprepare: Could not re-open display in 16 bit.");

		return false;
	}

#else// PP: if(BPLATFORM==PS2) else

	// PP: TEMP?: can't do focus effects on this platform at all :'o(
	return false;// PP: NB VFX_focusPrepared will have been false anyway so this code will never be reached

#endif// PP: (BPLATFORM==PS2)
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusNearTrackObject
	Purpose 	: near-focus on an object (things behind the object are blurred)
	Parameters 	: 
	Returns 	: 
	Info 		: uses distance from camera to object as focal length effective on other side
				got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusNearTrackObject(ACTORINSTANCE* const object)
{

#if(BPLATFORM==PS2)

	VFX_focusObject=object;
	VFX_focusMode=VFXFOCUS_NEAR;

	VFX_activateEffect(VFX_FOCUS);

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusFarTrackObject
	Purpose 	: far-focus on an object (things in front of the object are blurred)
	Parameters 	: 
	Returns 	: 
	Info 		: uses most of the distance from camera to object as focal length effective on near side
				got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusFarTrackObject(ACTORINSTANCE* const object)
{

#if(BPLATFORM==PS2)

	VFX_focusObject=object;
	VFX_focusMode=VFXFOCUS_FAR;

	VFX_activateEffect(VFX_FOCUS);

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusNear
	Purpose 	: set near-focus with the specified focal length (anything farther gets blurred)
	Parameters 	: focal length, (opt/true) whether or not to clear the focus object ptr
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusNear(const float focalLength, const bool clearObj)
{

#if(BPLATFORM==PS2)

	VFX_focusLength=focalLength;

	VFX_focusMode=VFXFOCUS_NEAR;

	if(clearObj)
	{
		VFX_focusObject=NULL;
	}

	VFX_focusNearVal=VFX_focusLength;
	VFX_focusFarVal=VFX_focusNearVal+VFX_focusLength;

	if(!VFX_effectActive(VFX_FOCUS))
	{
		VFX_activateEffect(VFX_FOCUS);
	}

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusFar
	Purpose 	: set far-focus with the specified focal length (anything closer gets blurred)
	Parameters 	: focal length, (opt/true) whether or not to clear the focus object ptr
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusFar(const float focalLength, const bool clearObj)
{

#if(BPLATFORM==PS2)

	VFX_focusLength=focalLength;

	VFX_focusMode=VFXFOCUS_FAR;

	if(clearObj)
	{
		VFX_focusObject=NULL;
	}

	VFX_focusFarVal=VFX_focusLength;
	VFX_focusNearVal=VFX_focusFarVal+VFX_focusLength;

	if(!VFX_effectActive(VFX_FOCUS))
	{
		VFX_activateEffect(VFX_FOCUS);
	}

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusBlur
	Purpose 	: focus on nothing (blur the whole scene)
	Parameters 	: 
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusBlur(void)
{

#if(BPLATFORM==PS2)

	VFX_focusMode=VFXFOCUS_BLUR;

	VFX_focusFarVal=0.0f;
	VFX_focusNearVal=0.0f;

	if(!VFX_effectActive(VFX_FOCUS))
	{
		VFX_activateEffect(VFX_FOCUS);
	}

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusUpdate
	Purpose 	: update camera focus
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusUpdate(void)
{

#if(BPLATFORM==PS2)

	// PP: track the object...

	if(VFX_focusObject != NULL)
	{
		// PP: update camera ptr
		if(camera[0].cameraCase != NULL)
		{
			VFX_focusCamera=camera[0].cameraCase->camera;
		}
		else
		{
			VFX_focusCamera= &camera[0];
		}

		VFX_focusLength=bmVectorDistance(VFX_focusCamera->pos, VFX_focusObject->actorInstance.position);
	}

	// PP: set up focus direction...

	if(VFX_focusMode == VFXFOCUS_NEAR)
	{
		VFX_focusNear(VFX_focusLength, false);
	}
	else if(VFX_focusMode == VFXFOCUS_FAR)
	{
		VFX_focusFar(VFX_focusLength, false);
	}

	// PP: apply focus
	bdSetDOFRange(VFX_focusNearVal, VFX_focusFarVal);

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusReset
	Purpose 	: turn off the focus effect
	Parameters 	: 
	Returns 	: 
	Info 		: NB this doesn't unprepare the display - call VFX_focusUnprepare when ye're done
*/
void VFX_focusReset(void)
{

#if(BPLATFORM==PS2)

	VFX_focusObject=NULL;

	VFX_focusBlurFactor=VFX_FOCUS_DEFAULT_BLUR_FACTOR;

	VFX_deactivateEffect(VFX_FOCUS);

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusSetBlurFactor
	Purpose 	: set the blur factor for focus effects
	Parameters 	: new blur factor for focus effects
	Returns 	: 
	Info 		: 
*/
void VFX_focusSetBlurFactor(const int blur)
{

#if(BPLATFORM==PS2)

	VFX_focusNextBlurFactor=blur;

#endif// PP: (BPLATFORM==PS2)

}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPreDraw
	Purpose 	: pre-draw for focus effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusPreDraw(void)
{
#if(BPLATFORM==PS2)
		
	for(int i=0;i<VFX_focusBlurFactor;i++)
	{
		bdDOFStart();
	}

#endif// PP: (BPLATFORM==PS2)
}


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPostDraw
	Purpose 	: post-draw for focus effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusPostDraw(void)
{
#if(BPLATFORM==PS2)
		
	for(int i=0;i<VFX_focusBlurFactor;i++)
	{
		bdDOFEnd();
	}

	VFX_focusBlurFactor=VFX_focusNextBlurFactor;

#endif// PP: (BPLATFORM==PS2)
}