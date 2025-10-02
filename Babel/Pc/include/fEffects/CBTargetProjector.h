// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBTargetProjector.h
//   Purpose : render target projector class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBTARGETPROJECTOR_H__
#define __FEFFECTS_CBTARGETPROJECTOR_H__

// ********************************************************************************
// Types, Structures and Classes

// shadowmap projector class
typedef class CBTargetProjector : public CBProjector {
	public:
		TBRenderTarget			*target;							// render target for shadow map

								// constructor
								CBTargetProjector(EBProjectorControl control, TBRenderTarget *targetPtr, uint32 flags);

								// destructor
		virtual					~CBTargetProjector();

								// enable this shadow projector on a texture stage
		virtual void			Enable(int stage=0);

								// incorporate the object matrix for the recieving geometry
		virtual void			SetRecievingGeometryMatrix(const TBMatrix m);

								// disable this shadow projector on a texture stage
		virtual void			Disable(int stage=0);
} CBTargetProjector;


#endif		// __FEFFECTS_CBTARGETPROJECTOR_H__
