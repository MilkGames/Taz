// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBShadowProjector.h
//   Purpose : shadowmap projector class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBSHADOWPROJECTOR_H__
#define __FEFFECTS_CBSHADOWPROJECTOR_H__

// ********************************************************************************
// Constants and macros

// shadow projector flags (for CBShadowProjector::DrawActorInstance)
#define BSHADOWPROJECTOR_CLEAR					0x00000001							// clear the shadow map before rendering
#define BSHADOWPROJECTOR_SETTARGET				0x00000002							// set the render target before rendering
#define BSHADOWPROJECTOR_UNSETTARGET			0x00000004							// unset the render target after rendering
#define BSHADOWPROJECTOR_CLEARANDDRAW			( BSHADOWPROJECTOR_CLEAR		| \
									  			  BSHADOWPROJECTOR_SETTARGET	| \
												  BSHADOWPROJECTOR_UNSETTARGET )	// combined flags to set the target, clear it, draw and unset it again

// shadow projector creation flags
#define BSHADOWPROJECTOR_ENABLEBLUR				0x00010000							// enable blur support
#define BSHADOWPROJECTOR_USEBLUR				0x00020000							// use blur
#define BSHADOWPROJECTOR_ENABLEANDUSEBLUR		( BSHADOWPROJECTOR_ENABLEBLUR | \
												 BSHADOWPROJECTOR_USEBLUR )			// combination of enable & use blur flags
#define BSHADOWPROJECTOR_ENABLEDITHER			0x00040000							// enable dithering on rendertarget

// shadow rendering style
typedef enum EBShadowProjectorStyle {
	EBSHADOWPROJECTORSTYLE_SECONDPASS,			// rendering a single shadow in a second pass
	EBSHADOWPROJECTORSTYLE_MULTISECONDPASS,		// rendering multiple shadows in a second pass
	EBSHADOWPROJECTORSTYLE_SINGLEPASS			// rendering a single shadow in a single pass
};


// ********************************************************************************
// Types, Structures and Classes

// shadowmap projector class
typedef class CBShadowProjector : public CBProjector {
	public:
		TBRenderTarget			*target;							// render target to use for shadow map
		TBActRenderMode_Shadow	shadowParms;						// shadow shader parameters
		EBShadowProjectorStyle	stageStyles[BMAX_TEXTURE_STAGES];	// render styles used

								// constructor
								CBShadowProjector(EBProjectorControl control, int xDim, int yDim, uint32 flags, int bpp=0);

								// destructor
		virtual					~CBShadowProjector();

								// draw an actor instance onto the shadow map
		void					DrawActorInstance(TBActorInstance *actInstance, uint32 alphaFlags, uint32 flags);

								// set the shadow density
		void					SetShadowDensity(int density);

								// enable this shadow projector on a texture stage
		virtual void			Enable(int stage=0);
		void					Enable(int stage, EBShadowProjectorStyle renderStyle);

								// incorporate the object matrix for the recieving geometry
		virtual void			SetRecievingGeometryMatrix(const TBMatrix m);

								// disable this shadow projector on a texture stage
		virtual void			Disable(int stage=0);

								// blur the shadow map associated with this projector
		void					Blur(int noofPasses);
} CBShadowProjector;


// ********************************************************************************
// Globals


#endif		// __FEFFECTS_CBSHADOWPROJECTOR_H__
