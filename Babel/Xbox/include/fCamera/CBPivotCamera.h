// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBPivotCamera.h
//   Purpose : pivot around a point camera class
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FCAMERA_CBPIVOTCAMERA_H__
#define __FCAMERA_CBPIVOTCAMERA_H__

// ********************************************************************************
// Constants and macros

// Fly Around camera flags
#define BPIVOTCAMERA_XINVERT	0x00000001			// invert X movement
#define BPIVOTCAMERA_YINVERT	0x00000002			// invert Y movement


// ********************************************************************************
// Types, Structures and Classes

// camera base class
typedef class CBPivotCamera : public CBCameraBase {
	public:
		TBVector					focus;							// focus point world position
		float						xRot, yRot, zRot;				// orientation
		float						collisionRadius;				// collision radius (0 = no collision)
		float						speed;							// normal movement speed
		float						distance;						// distance from pivot
		float						minDistance, maxDistance;		// min distance, max distance
		TBIInputChannel				*chLeftRight;					// look left/right input channel
		TBIInputChannel				*chUpDown;						// look up/down input channel
		TBIInputChannel				*chForwardBack;					// forward/back movement channel
		TBIInputChannel				*chTurbo, *chSlow;				// turbo (*5) and slow (/5) speed modifier channels
		TBIChannelMap				*channelMap;					// channel map for auto-bindings
		uint32						flags;							// flags

									CBPivotCamera();				// constructor
		virtual						~CBPivotCamera();				// destructor

									// set the info for this camera
		void						SetInfo(float x, float y, float z, float xRot=0.0f, float yRot=0.0f,
																		float collisionRadius=0.0f, float speed=1.0f, uint32 flags=0);

									// set the camera's focus point
		void						SetFocus(float x, float y, float z);

									// set the camera's orientation
		void						SetOrientation(float xRot, float yRot, float zRot=0.0f);

									// set the camera's tilt on its own
		void						SetTilt(float vZRot);

									// set the camera's distance and range
		void						SetDistance(float distance, float minDistance, float maxDistance);

									// set the input channels for this camera
		void						SetChannels(TBIInputChannel *chLeftRight, TBIInputChannel *chUpDown, TBIInputChannel *chForwardBack,
																			TBIInputChannel *chTurbo=NULL, TBIInputChannel *chSlow=NULL);

									// set auto-channel bindings for this camera
		void						SetupAutoBindings(int padId=0);

									// update the camera
		virtual void				Update(const float t=1.0f);
} CBPivotCamera;


#endif		// __FCAMERA_CBPIVOTCAMERA_H__
