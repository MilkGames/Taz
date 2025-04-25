// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBCameraBase.h
//   Purpose : base class for camera systems
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FCAMERA_CBCAMERABASE_H__
#define __FCAMERA_CBCAMERABASE_H__

// ********************************************************************************
// Types, Structures and Classes

// camera base class
typedef class CBCameraBase {
	public:
		TBCameraInstance			cameraInstance;						// camera instance being used by this system
		UBContext32					context;							// client context value union

									CBCameraBase();						// constructor
		virtual 					~CBCameraBase();					// destructor

									// update the camera
		virtual void				Update(const float t=1.0f) = 0;

									// make this the active camera
		TBCameraInstance			*Activate();
} CBCameraBase;


#endif		// __FCAMERA_CBCAMERABASE_H__
