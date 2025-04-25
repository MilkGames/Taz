// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : VideoFX.cpp
//   Purpose : Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_VIDEOFX
#define __H_VIDEOFX

#include "rectangle.h"


// PP: video effects
enum EVideoEffect
{
	VFX_NONE			=0,

	VFX_STEREO,					// PP: stereo vision
	VFX_MOTIONBLUR,				// PP: motion blur
	VFX_BLUR,					// PP: defocus
	VFX_ZOOM,					// PP: video zoom
	VFX_SHADOWMAP,				// PP: shadow map
	VFX_SPINOFF,				// PP: screen spins off to reveal next shot underneath (not ready yet)
	VFX_TIMESLICE,				// PP: Matrix cam (freezes time and rotates the camera around a point)
	VFX_FOCUS,					// PP: camera focus (depth-of-field)
	VFX_TRANSITION,				// NH: transitional effect (ie. page peel)

	NUM_VFX

}ENUMOPS(EVideoEffect);


// PP: WILL THIS PLATFORM USE A PERMANENT VFX_renderTarget?  GO ON, YOU KNOW YOU WANT TO! ...
// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if(BPLATFORM == PS2)

#define VFX_PERMANENT_RENDER_TARGET		false							// PP: speed hit

#elif(BPLATFORM == XBOX)

#define VFX_PERMANENT_RENDER_TARGET		true							// PP: not yet tested for speed hit

#elif(BPLATFORM == PC)

#define VFX_PERMANENT_RENDER_TARGET		true							// PP: not yet tested for speed hit

#elif(BPLATFORM == GAMECUBE)

//#error Could the Gamecube keep a permanent screen-sized offscreen render target (for effects), without causing a speed hit?
#define VFX_PERMANENT_RENDER_TARGET		false							// Just memory allocation !

#else// PP: unsupported platform

#error Could this platform keep a permanent screen-sized offscreen render target (for effects), without causing a speed hit?
// PP: #define VFX_PERMANENT_RENDER_TARGET		true/false?

#endif// PP: platform
// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


extern unsigned char			VFX_flipAlpha;					// PP: alpha with which to 'flip' the screen (bdFlip)

// PP: multi-purpose render target used for video effects
extern TBRenderTarget*			VFX_renderTarget;

// PP: info aboot wur render target
struct RENDERTARGETINFO
{
	int		width;
	int		height;
	int		rgbDepth;
	int		zDepth;
};

// PP: info about the VFX_renderTarget
extern RENDERTARGETINFO			VFX_renderTargetInfo;

// PP: small render target with private z buffer, for shadow maps
extern TBRenderTarget*			VFX_shadowRenderTarget;


/*	--------------------------------------------------------------------------------
	Function 	: VFX_bookPermanentRenderTarget
	Purpose 	: specify that the VFX_renderTarget should NOT be freed when out of use
	Parameters 	: 
	Returns 	: 
	Info 		: eg. in the West Boss, where the target gets used a lot for motion blur and we want to prevent fragmentation)  // PP: TODO?: keep a permanent render target right the way through the game on XBOX/PC?
					NOTE: this does not actually create the render target
*/
void VFX_bookPermanentRenderTarget(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_permanentRenderTarget
	Purpose 	: release the render target that was booked as a permanent target by VFX_bookPermanentRenderTarget
	Parameters 	: 
	Returns 	: 
	Info 		: doesn't try to release the target if it hasn't been created
*/
void VFX_releasePermanentRenderTarget(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_effectActive
	Purpose 	: determine if a particular effect is active
	Parameters 	: effect to check for
	Returns 	: true if the effect is active, false if not
	Info 		: 
*/
bool VFX_effectActive(const EVideoEffect effect);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_activateEffect
	Purpose 	: activate an effect, switch it on
	Parameters 	: index of effect to activate, (opt/false) priority flag
	Returns 	: true if the activation succeeded, false if not
	Info 		: 
*/
bool VFX_activateEffect(const EVideoEffect effect, const bool priority=false);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_deactivateEffect
	Purpose 	: deactivate an effect, switch it off
	Parameters 	: index of effect to deactivate
	Returns 	: 
	Info 		: *** THIS ASSUMES THAT THE APPROPRIATE CHECKS HAVE ALREADY BEEN DONE TO PROTECT CHEAT EFFECTS ***
*/
void VFX_deactivateEffect(const EVideoEffect effect);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_init
	Purpose 	: Initialise video effects
	Parameters 	: 
	Returns 	: 
	Fact 		: #1. The theme music to the popular children's television programme 
						"Button Moon" was sung by Peter Davison and Sandra Dickinson.
*/
void VFX_init(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initScene
	Purpose 	: Initilaisation to be done as each scene is prepared
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initScene(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_update
	Purpose 	: udpate video effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_draw
	Purpose 	: normal draw for video effects
	Parameters 	: 
	Returns 	: 
	Info 		: some effects don't get drawn by this function becuase their draw
					has to be at some particular point, eg. before or after any
					other drawing
*/
void VFX_draw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_shutdown
	Purpose 	: Shutdown video effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_shutdown(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												STEREOSCOPIC MODE

				Dig out those funky glasses, or make a pair with cardboard and some sweetie wrappers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: types on anaglyph that we generate in sterescopic mode
enum EAnaglyphType
{
	ANAGLYPH_RED_CYAN,		// PP: Red on the left eye, cyan on the right.  This seems to be the most common arrangement.
	ANAGLYPH_CYAN_RED,		// PP: Cyan on the left eye, red on the right.  This seems slightly less common.

	NUM_ANAGLYPH_TYPES		// PP: number of supported anaglyph types
};

// PP: the type of anaglyph to be generated by the stereoscopic mode
extern EAnaglyphType			VFX_stereoAnaglyphType;

// PP: which eye's view is being drawn?
extern int						VFX_stereoEye;

// PP: do we use the VFX_renderTarget for the stereo effect?  We don't have to you know...
extern bool						VFX_stereoUseRenderTarget;


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initStereo
	Purpose 	: Initialise stereoscopic mode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initStereo(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_setStereo
	Purpose 	: turn the stereoscopic mode on or off
	Parameters 	: on/off, de-saturate colours, (opt/false) priority flag
	Returns 	: 
	Info 		: looks silly I know, but makes sense I think

				*** NOTE: the 'priority' flag is used to indicate that the stereo vision is being activated/dectivated by a stereo vision cheat.
					When using stereo vision (for whatever wacky reason) within the game, please leave the priority flag with its default value of false.
*/
void VFX_setStereo(const bool on, const bool grey, const bool priority=false);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_stereoPreDraw
	Purpose 	: Prepare to draw a scene stereoscopically
	Parameters 	: 
	Returns 	: 
	Info 		: (second eye only) sets the VFX_renderTarget as the current render target
*/
void VFX_stereoPreDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_stereoPostDraw
	Purpose 	: render the stereoscopic image
	Parameters 	: 
	Returns 	: true if the draw loop is to be repeated
	Info 		: 
*/
bool VFX_stereoPostDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_adjustStereoCamera
	Purpose 	: Adjust a camera's position and orientation for stereoscopic effect
	Parameters 	: [in/out] ref to camera position VEC, [in/out] camera orientation quaternion
	Returns 	: 
	Info 		: 
*/
void VFX_adjustCameraForStereoscopy(VEC& camPos, TBQuaternion camQuat);


/*	--------------------------------------------------------------------------------
	Function 	: DrawStereoscopyFilter
	Purpose 	: Apply a colour filter over EVERYTHING that has been drawn,
					allowing through only the colour of the eye
	Parameters 	: 
	Returns 	: 
	Info 		: USE THIS ON THE FIRST EYE ONLY
*/
void VFX_drawStereoFilter(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_applyStereoColourStrengths
	Purpose 	: alter the strength(s) of the colour channel(s) of this eye's
					draw pass; thus reducing ghosting
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_applyStereoColourStrengths(void);


// PP: ~~~~~~~~~~~~


/*	--------------------------------------------------------------------------------
	Function 	: VFX_setBlur
	Purpose 	: Set the level of blurring to use
	Parameters 	: (opt/0.0f=off) level of blur
	Returns 	: 
	Info 		: I'm thinking this will work by resizing the viewport, drawing the scene, then zooming in on the used area of the render target.  ie. it will work purely by filtering?
*/
void VFX_setBlur(const float blur=0.0f);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MOTION BLUR

							Full screen only at the moment, I don't know if we need anything else?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: MOVEME?: normal ingame motion blur level
#define NORMAL_INGAME_MOTION_BLUR					0.0f// PP: none

// PP: On platforms other than PS2, use the VFX_renderTarget for motion blur (on PS2 it's kinda 'free' or something, I dunno)
#if (BPLATFORM != PS2)
#define VFX_MOTIONBLUR_USE_RENDER_TARGET
#endif // PP: (BPLATFORM != PS2)


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlur
	Purpose 	: Add to the motion blur level for this frame
	Parameters 	: level of motion blur to add: 0.0f(none [ignored])..1.0f(all [image doesn't update])
	Returns 	: 
	Info 		: // PP: motion blur level is set to zero at the beginning of each frame,
					and is then added to by any number of things throughout the frame, using this function
*/
void VFX_motionBlur(const float blur);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlurPreDraw
	Purpose 	: Prepare to draw with motion blur
	Parameters 	: 
	Returns 	: 
	Info 		: (platforms other than PS2) sets the VFX_renderTarget as the current render target
*/
void VFX_motionBlurPreDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_motionBlurPostDraw
	Purpose 	: draw with motion blur, having rendered the scene
	Parameters 	: 
	Returns 	: 
	Info 		: (platforms other than PS2) blends the current frame onto the previous frames
*/
void VFX_motionBlurPostDraw(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											BLURRED CHARACTER SHADOW MAPS

													PS2-friendly
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: VFX_createShadowRenderTarget
	Purpose 	: create the render target for shadow maps
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_createShadowRenderTarget(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_releaseShadowRenderTarget
	Purpose 	: release the VFX_shadowRenderTarget
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_releaseShadowRenderTarget(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initShadowMap
	Purpose 	: initialise shadow maps
	Parameters 	: 
	Returns 	: 
	Info 		: sets up verts for blur stage
*/
void VFX_initShadowMap(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_generateShadowMap
	Purpose 	: Create a shadow map of a character
	Parameters 	: ptr ACTORINSTANCE whose shadow map to generate
	Returns 	: 
	Info 		: // PP: TODO: optimise the shadow system
*/
void VFX_generateShadowMap(ACTORINSTANCE* const actorInstance);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_shadowMapPostDrawDebug
	Purpose 	: display the final shadow map
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_shadowMapPostDrawDebug(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													SCREEN SPIN-OFF

		Takes a snapshot of the scene, and quickly spins it off of the screen to reveal the next shot underneath.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: NOT USED YET
enum VFXSpinoffType
{
	VFXSPINOFFTYPE_SPIN,		// PP: screen spins off - it's the classic tacky effect you see at the end of sports replays & stuff

	// PP: a few ideas...

	VFXSPINOFFTYPE_PEEL,		// PP: screen peels off, like a sheet of paper being torn off a notepad
	VFXSPINOFFTYPE_COLLAPSE,	// PP: screen drops off untidily, like a badly hung painting falling off a wall
	VFXSPINOFFTYPE_FADE,		// PP: screen fades to transparency
	VFXSPINOFFTYPE_DRIVEOFF,	// PP: reeely tacky - the screen h-shears and drives off like a hot rod or something
	VFXSPINOFFTYPE_IMAGESWITHINPIXELS
};

// PP: most of these chaps get updated from the game loops (main.cpp)...

extern bool			VFX_spinoffPrePreparing;		// PP: spinoff will get prepared over the next frame
extern bool			VFX_spinoffPreparing;			// PP: spinoff is being prepared over this frame
extern bool			VFX_spinoffNewlyPrepared;		// PP: spinoff is newly prepared and ready to start
extern bool			VFX_spinoffPrepared;			// PP: spinoff is prepared and ready to start
extern bool			VFX_spinoffNewlyStarted;		// PP: spinoff has just started - could check for this before changing camera shots
extern bool			VFX_spinoffStarted;				// PP: spinoff has started - could check for this before changing camera shots


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPreparePreDraw
	Purpose 	: one-time pre-draw for spinoff
	Parameters 	: 
	Returns 	: 
	Info 		: sets the VFX_renderTarget as the current render target
*/
void VFX_spinoffPreparePreDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPrepare
	Purpose 	: prepare a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This could be called one frame before a change of camera shot
					Sets the VFX_renderTarget as the current render target
*/
void VFX_spinoffPrepare(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffStart
	Purpose 	: start a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This can be called at any time once VFX_spinoffPrepared is true
*/
void VFX_spinoffStart(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffUpdate
	Purpose 	: update a spin-off
	Parameters 	: 
	Returns 	: 
	Info 		: This gets called by VFX_update while VFX_effectActive(VFX_SPINOFF)
*/
void VFX_spinoffUpdate(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPostPrepare
	Purpose 	: finish preparing a spinoff
	Parameters 	: 
	Returns 	: 
	Info 		: This is called from game loops (main.cpp)
*/
void VFX_spinoffPostPrepare(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_spinoffPostDraw
	Purpose 	: draw a spin-off effect in progress
	Parameters 	: 
	Returns 	: 
	Info 		: This is called from game loops (main.cpp) while VFX_effectActive(VFX_SPINOFF)
*/
void VFX_spinoffPostDraw(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TIMESLICE

									Freezes time and rotates the camera around a point.

										'Flow-Motion™" or 'Matrix-Cam™' if you insist.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern float	VFX_timesliceTimer;		// PP: timer you can use to delay triggering timeslice
extern bool		VFX_timesliceLatch;		// PP: latch you can use to control triggering timeslice


/*	--------------------------------------------------------------------------------
	Function 	: VFX_timeslice
	Purpose 	: start a timeslice
	Parameters 	: 
	Returns 	: 
	Info 		: uses camera[0], starts from its current position and rotates 90° around its focus point
					Might make this a bit more customisable at some point
					
*/
void VFX_timeslice();


/*	--------------------------------------------------------------------------------
	Function 	: VFX_timesliceDraw
	Purpose 	: draw for the timeslice effect
	Parameters 	: 
	Returns 	: 
	Info 		: applies lightness errors	
*/
void VFX_timesliceDraw(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															FOCUS

								Camera focus effects, courtesy of the lovely Mr. Sidwell
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: VFX_initFocus
	Purpose 	: one-time init for focus effects, called by VFX_init
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_initFocus(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPrepare
	Purpose 	: Prepare the display for focus effects
	Parameters 	: 
	Returns 	: true if the preparation was successful, false otherwise
	Info 		: puts the display into 32 bit
*/
bool VFX_focusPrepare(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusUnprepare
	Purpose 	: Reset the display after focus effects
	Parameters 	: 
	Returns 	: true if the reset was successful, false otherwise
	Info 		: puts the display back into 16 bit
*/
bool VFX_focusUnprepare(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusNearTrackObject
	Purpose 	: near-focus on an object (things behind the object are blurred)
	Parameters 	: 
	Returns 	: 
	Info 		: uses distance from camera to object as focal length effective on other side
				got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusNearTrackObject(ACTORINSTANCE* const object);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusFarTrackObject
	Purpose 	: far-focus on an object (things in front of the object are blurred)
	Parameters 	: 
	Returns 	: 
	Info 		: uses most of the distance from camera to object as focal length effective on near side
				got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusFarTrackObject(ACTORINSTANCE* const object);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusNear
	Purpose 	: set near-focus with the specified focal length (anything farther gets blurred)
	Parameters 	: focal length, (opt/true) whether or not to clear the focus object ptr
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusNear(const float focalLength, const bool clearObj=true);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusFar
	Purpose 	: set far-focus with the specified focal length (anything closer gets blurred)
	Parameters 	: focal length, (opt/true) whether or not to clear the focus object ptr
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusFar(const float focalLength, const bool clearObj=true);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusBlur
	Purpose 	: focus on nothing (blur the whole scene)
	Parameters 	: 
	Returns 	: 
	Info 		: got to prepare the display first with VFX_focusPrepare
*/
void VFX_focusBlur(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusUpdate
	Purpose 	: update camera focus
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusUpdate(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusReset
	Purpose 	: turn off the focus effect
	Parameters 	: 
	Returns 	: 
	Info 		: NB this doesn't unprepare the display - call VFX_focusUnprepare when ye're done
*/
void VFX_focusReset(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusSetBlurFactor
	Purpose 	: set the blur factor for focus effects
	Parameters 	: new blur factor for focus effects
	Returns 	: 
	Info 		: 
*/
void VFX_focusSetBlurFactor(const int blur);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPreDraw
	Purpose 	: pre-draw for focus effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusPreDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: VFX_focusPostDraw
	Purpose 	: post-draw for focus effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void VFX_focusPostDraw(void);


#endif // PP: ndef __H_VIDEOFX