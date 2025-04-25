// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBFlyAroundCamera.h
//   Purpose : fly around camera class
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FCAMERA_CBFLYAROUNDCAMERA_H__
#define __FCAMERA_CBFLYAROUNDCAMERA_H__

// ********************************************************************************
// Constants and macros

// Fly Around camera flags
#define BFLYAROUNDCAMERA_YINVERT	0x00000001


// ********************************************************************************
// Types, Structures and Classes

// camera base class
typedef class CBFlyAroundCamera : public CBCameraBase {
	public:
		TBVector					position;						// world position
		float						xRot, yRot, zRot;				// orientation
		float						collisionRadius;				// collision radius (0 = no collision)
		float						speed;							// normal movement speed
		TBIInputChannel				*chLeftRight;					// look left/right input channel
		TBIInputChannel				*chUpDown;						// look up/down input channel
		TBIInputChannel				*chForwardBack;					// forward/back movement channel
		TBIInputChannel				*chStrafe;						// strafe movement channel
		TBIInputChannel				*chTurbo, *chSlow;				// turbo (*5) and slow (/5) speed modifier channels
		TBIChannelMap				*channelMap;					// channel map for auto-bindings
		uint32						flags;							// flags

									CBFlyAroundCamera();			// constructor
		virtual						~CBFlyAroundCamera();			// destructor

									// set the info for this camera
		void						SetInfo(float x, float y, float z, float xRot=0.0f, float yRot=0.0f, float collisionRadius=0.0f,
																										float speed=1.0f, uint32 flags=0);

									// set the camera's position
		void						SetPosition(float x, float y, float z);

									// set the camera's orientation
		void						SetOrientation(float xRot, float yRot, float zRot=0.0f);

									// set the camera's tilt on its own
		void						SetTilt(float vZRot);

									// set the input channels for this camera
		void						SetChannels(TBIInputChannel *chLeftRight, TBIInputChannel *chUpDown, TBIInputChannel *chForwardBack,
												TBIInputChannel *chStrafe, TBIInputChannel *chTurbo=NULL, TBIInputChannel *chSlow=NULL);

									// set auto-channel bindings for this camera
		void						SetupAutoBindings(int padId=0);

									// update the camera
		virtual void				Update(const float t=1.0f);
} CBFlyAroundCamera;


#endif		// __FCAMERA_CBFLYAROUNDCAMERA_H__
