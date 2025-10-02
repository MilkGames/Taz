// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBProjector.h
//   Purpose : projector base class class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBPROJECTOR_H__
#define __FEFFECTS_CBPROJECTOR_H__

// ********************************************************************************
// Constants and macros

// projector control mode
typedef enum EBProjectorControl {
	EBPROJECTORCONTROL_POSITIONAL,						// projector is specified as position in space
	EBPROJECTORCONTROL_ORIENTATION,						// projector is specified as orientation around focus
};


// projector flags
#define BPROJECTOR_DERIVEDMASK				0xffff0000			// mask for derived class flags
#define BPROJECTOR_USESTENCIL				0x00000001			// use the stencil buffer to mask the projected area
#define BPROJECTOR_STICKYFOCUS				0x00000002			// focus has been attached to an actor instance
#define BPROJECTOR_STICKYEULER				0x00000004			// sticky euler angle orientation
#define BPROJECTOR_STICKYQUAT				0x00000008			// sticky quaternion orientation
#define BPROJECTOR_STICKYMATRIX				0x00000010			// sticky matrix orientation
#define BPROJECTOR_STICKYORIENTATIONMASK	(BPROJECTOR_STICKYEULER		| \
											 BPROJECTOR_STICKYQUAT		| \
											 BPROJECTOR_STICKYMATRIX)	// mask for sticky orientation flags
#define BPROJECTOR_STICKYPOSITION			0x00000020			// sticky matrix orientation


// ********************************************************************************
// Types, Structures and Classes

// projector base class
typedef class CBProjector {
	private:
									// update the position/orientation of the projector from it's inputs
		void						UpdateOrientationPosition();


	protected:
									// draw the mask frustum into the stencil buffer to limit projected rendering
		void						DrawMaskFrustum(TBMatrix projView);

									// recalculate the projection matrix
		void						CalcProjectionMatrix();


	public:
		EBProjectorControl			controlMode;				// how this projector is being controlled
		TBVector					focus;						// destination (i.e. focus) position
		TBVector					position;					// source position (if used in positional mode)
		TBMatrix					orientation;				// orientation matrix (if used in orientation mode) (rotates world space points infront of the projectortakes points from world space into camera space when coupled with positional translation)
		TBMatrix					invOrientation;				// inverse orientation matrix (rotates points from projector space into world space)
		float						distance;					// distance of projector from focus (if used in orientation mode)
		uint32						projectorFlags;				// flags bitfield (see BPROJECTOR_)
		TBActorInstance				*focusActor;				// actor we are focused on
		TBVector					focusOffset;				// offset for focal point from actor origin
		float						*stickyFocus;				// vector focus point is tracking
		float						*stickyPosition;			// vector position is tracking
		union {
			float					*stickyQuat;				// ptr to quaternion our orientation is tracking
			float					*stickyMatrix;				// ptr to matrix our orientation is tracking
			float					*stickyEuler[3];			// ptr to euler angles our orientation is tracking
		};
		float						xFov, yFov;					// X and Y field of view angles in radians
		float						xFovProj, yFovProj;			// 1/tan(0.5f*fov) for projection matrix
		float						nearClip, farClip;			// near and far clip planes for the projector
		TBMatrix					projProjection;				// projector projection matrix
		float						uScale, vScale;				// scale dimensions in u and v of projection matrix
		float						roll;						// roll in radians for positional projectors

		int							stageSet;					// stage set on
		TBMatrix					viewToProjector;			// view to projection matrix

		UBContext32					context;					// client context value

									// constructor
									CBProjector(EBProjectorControl control, uint32 flags);

									// destructor
		virtual 					~CBProjector();

									// attach the focus of this texture projector to an actor instance
		void						FocusOnActor(TBActorInstance *actInstance, TBVector offset, int sticky);

									// set the focus point for this projector
		void						SetFocus(TBVector focalPoint, int sticky=FALSE);

									// set the focus point for this projector
		void						SetFocus(float x,float y,float z);

									// set the position of this projector
		void						SetPosition(TBVector pos, int sticky=FALSE);

									// set the position of this projector
		void						SetPosition(float x, float y, float z);

									// set the orientation of this projector from sticky euler angles
		void						SetOrientation(float *xAngRadians, float *yAngRadians, float *zAngRadians, float dist=0.0f);

									// set the orientation of this projector from euler angles
		void						SetOrientation(float xAngRadians, float yAngRadians, float zAngRadians, float dist=0.0f);

									// set the orientation of this projector from a quaternion
		void						SetOrientation(TBQuaternion orientation, float dist=0.0f, int sticky=FALSE);

									// set the orientation of this projector from a matrix
		void						SetOrientation(TBMatrix orientation, float dist=0.0f, int sticky=FALSE);

									// update this projector from it's sticky values
		void						Update();

									// set FOV and clipplane values for this projector
		void						SetViewInfo(float xFovVal, float yFovVal, float nearClipVal, float farClipVal);

									// display the frustum of this projector
		void						Show();

									// set the distance an orientation based projector is from it's focus
		void						SetDistance(float distance);

									// enable this shadow projector on a texture stage
		virtual void				Enable(int stage=0) = 0;

									// incorporate the object matrix for the recieving geometry
		virtual void				SetRecievingGeometryMatrix(const TBMatrix m) = 0;

									// disable this shadow projector on a texture stage
		virtual void				Disable(int stage=0) = 0;
} CBProjector;


// Globals

extern CBProjector *bActiveProjector[BMAX_TEXTURE_STAGES];	// Internal use only


#endif		// __FEFFECTS_CBPROJECTOR_H__
