// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBTextureProjector.h
//   Purpose : texture projection class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBTEXTUREPROJECTOR_H__
#define __FEFFECTS_CBTEXTUREPROJECTOR_H__

// ********************************************************************************
// Constants and macros

// texture projector flags
#define BTEXTUREPROJECTOR_TILE			0x00010000				// tile this projection instead of clamping it


// ********************************************************************************
// Types, Structures and Classes

// texture projector class
typedef class CBTextureProjector : public CBProjector {
	public:
		TBTexture				*texture;						// texture being projected

								// constructor
								CBTextureProjector(EBProjectorControl control, TBTexture *texturePtr, uint32 flags);

								// destructor
		virtual					~CBTextureProjector();

								// enable this shadow projector on a texture stage
		virtual void			Enable(int stage=0);

								// incorporate the object matrix for the recieving geometry
		virtual void			SetRecievingGeometryMatrix(const TBMatrix m);

								// disable this shadow projector on a texture stage
		virtual void			Disable(int stage=0);

								// set the tiling for the texture
		void					SetTile(float uTile, float vTile);

								// change projected texture
		void					SetTexture(TBTexture *texturePtr);

} CBTextureProjector;


#endif		// __FEFFECTS_CBTEXTUREPROJECTOR_H__
