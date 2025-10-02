// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBAnimSplashRenderer.h
//   Purpose : animated splash screen renderer
// Component : Specific BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBANIMSPLASHRENDERER_H__
#define __FEFFECTS_CBANIMSPLASHRENDERER_H__


// ********************************************************************************
// Types, Structures and Classes

// animated splash screen base class
typedef class CBAnimSplashRenderer {
	private:
		TBCameraInstance			*oldCamera;							// preserved camera
		float						oldViewX, oldViewY, oldViewZ;		// preserved viewpoint position
		float						oldViewXFov, oldViewYFov;			// preserved field of view
		float						oldViewNearClip, oldViewFarClip;	// preserved clip planes
		TBMatrix					oldViewOrientation;					// preserved viewpoint orientation
		float						uScale, vScale;						// texture coord scale factors
		TBPrimVertex				*verts;								// rendering vert buffer

	public:
									// constructor
									CBAnimSplashRenderer();

									// destructor
									~CBAnimSplashRenderer();

									// render the current attribute table
		void						Render(class CBAnimSplash *splash);

									// enter the renderer
		int							Enter();

									// exit the renderer
		void						Exit();

									// set the texture coordinate scale factors
		void						SetUVScale(float u, float v);
} CBAnimSplashRenderer;

#endif		// __FEFFECTS_CBANIMSPLASHRENDERER_H__
