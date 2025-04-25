// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : camera.h
//   Purpose : camera control code
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_CAMERA_H__
#define __BACTOR_CAMERA_H__

// ********************************************************************************
// Constants and Macros

// camera control modes
typedef enum EBCameraCtrlMode {
	BCAMERACTRL_POSITION,			// fixed vector position
	BCAMERACTRL_TRACKING_POSITION,	// track a vector position
	BCAMERACTRL_ACTOR,				// track an actor
	BCAMERACTRL_QUATERNION,			// quaternion based rotation
	BCAMERACTRL_TRACKING_QUATERNION,// track a quaternion based rotation
	BCAMERACTRL_EULER,				// euler based rotation
	BCAMERACTRL_TRACKING_EULER,		// track a euler based rotation
	BCAMERACTRL_CUSTOM				// callback controlled
};

#define BCAMERACTRL_STATIC				BCAMERACTRL_POSITION
#define BCAMERACTRL_VECTOR				BCAMERACTRL_TRACKING_POSITION


// camera instance flags
#define BCAMERAFLAG_DYNAMIC			0x00000001				// control struct has been dynamically allocated


// camera info flags
#define BCAMERAINFO_XFOV			0x00000001
#define BCAMERAINFO_YFOV			0x00000002
#define BCAMERAINFO_NEARCLIP		0x00000004
#define BCAMERAINFO_FARCLIP			0x00000008
#define BCAMERAINFO_FOV				(BCAMERAINFO_XFOV | BCAMERAINFO_YFOV)
#define BCAMERAINFO_CLIP			(BCAMERAINFO_NEARCLIP | BCAMERAINFO_FARCLIP)
#define BCAMERAINFO_ALL				0xffffffff


// ********************************************************************************
// Types, Structures and Classes

// BCAMERACTRL_STATIC control mode parms
typedef struct _TBCameraInstanceControlStatic {
	float									x, y, z;				// position of point
	float									roll;					// camera roll (radians)
} TBCameraInstanceControlStatic;


// BCAMERACTRL_VECTOR control mode parms
typedef struct _TBCameraInstanceControlVector {
	float									*vector;				// ptr to vector to track
	float									roll;					// camera roll (radians)
} TBCameraInstanceControlVector;


// BCAMERACTRL_ACTOR control mode parms
typedef struct _TBCameraInstanceControlActor {
#ifdef __BACTOR_ACTOR_H__
	TBActorInstance							*actorInstance;			// ptr to actor instance to track
	TBActorNodeInstance						*nodeInstance;			// ptr to actor node or NULL for actor world space
#else
	void									*actorInstance;			// ptr to actor instance to track
	void									*nodeInstance;			// ptr to actor node or NULL for actor world space
#endif
	float									x, y, z;				// relative position in one of above spaces
	float									roll;					// camera roll (radians)
} TBCameraInstanceControlActor;

// BCAMERACTRL_QUATERNION control mode parms
typedef struct _TBCameraInstanceControlQuaternion {
	TBQuaternion							quaternion;				// quaternion to use
} TBCameraInstanceControlQuaternion;


// BCAMERACTRL_TRACKING_QUATERNION control mode parms
typedef struct _TBCameraInstanceControlTQuaternion {
	float									*quaternion;			// quaternion to use
} TBCameraInstanceControlTQuaternion;


// BCAMERACTRL_EULER control mode parms
typedef struct _TBCameraInstanceControlEuler {
	float									xr, yr, zr;				// rotations
	EBViewportRotationOrder					order;					// rotation order
} TBCameraInstanceControlEuler;


// BCAMERACTRL_TRACKING_EULER control mode parms
typedef struct _TBCameraInstanceControlTEuler {
	float									*angle;					// ptr to vector to track
	EBViewportRotationOrder					order;					// rotation order
} TBCameraInstanceControlTEuler;


// client callback for camera BCAMERACTRL_CUSTOM control mode
typedef void (*TBCameraInstanceCallback)(TBVector position, float *roll, void *context);


// BCAMERACTRL_CUSTOM control mode parms
typedef struct _TBCameraInstanceControlCustom {
	TBCameraInstanceCallback				callback;				// callback function
	void									*context;				// context
} TBCameraInstanceControlCustom;


// a camera instance control
typedef struct _TBCameraInstanceControl {
	union {
		TBCameraInstanceControlStatic		staticParms;			// parameters for BCAMERACTRL_STATIC control mode
		TBCameraInstanceControlVector		vectorParms;			// parameters for BCAMERACTRL_VECTOR control mode
		TBCameraInstanceControlActor		actorParms;				// parameters for BCAMERACTRL_ACTOR control mode
		TBCameraInstanceControlQuaternion	quaternionParms;		// parameters for BCAMERACTRL_QUATERNION control mode
		TBCameraInstanceControlTQuaternion	tQuaternionParms;		// parameters for BCAMERACTRL_TRACKING_QUATERNION control mode
		TBCameraInstanceControlEuler		eulerParms;				// parameters for BCAMERACTRL_EULER control mode
		TBCameraInstanceControlTEuler		tEulerParms;			// parameters for BCAMERACTRL_TRACKING_EULER control mode
		TBCameraInstanceControlCustom		customParms;			// parameters for BCAMERACTRL_CUSTOM control mode
		char								pad[28];				// pad union to 28 bytes so overall structure makes 32 bytes
	};
	EBCameraCtrlMode						ctrlMode;				// control mode
} TBCameraInstanceControl;


// a camera instance
typedef struct _TBCameraInstance {
	TBCameraInstanceControl					source;					// source control
	TBCameraInstanceControl					target;					// target control
	uint32									flags;					// camera flags
	UBContext32								context;				// client context union
	float									xFov, yFov;				// field of view
	float									nearClip, farClip;		// clip distances
	struct _TBCameraInstance				*next, *prev;			// list links
} TBCameraInstance;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitCamera
	Purpose : initialise the camera submodule
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitCamera();


/*	--------------------------------------------------------------------------------
	Function : bShutdownCamera
	Purpose : shutdown the camera submodule
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownCamera();


/*	--------------------------------------------------------------------------------
	Function : bUpdateViewFromCamera
	Purpose : update the view properties from the current camera (if there is one)
	Parameters : 
	Returns : 1=set, 0=no camera set
	Info : 
*/

int bUpdateViewFromCamera();


/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstance
	Purpose : set the current camera instance
	Parameters : ptr to camera instance
	Returns : ptr to old camera instance
	Info : 
*/

TBCameraInstance *baSetCameraInstance(TBCameraInstance *camera);


/*	--------------------------------------------------------------------------------
	Function : baGetCurrentCameraInstance
	Purpose : get the current camera instance
	Parameters : 
	Returns : ptr to current camera instance
	Info : 
*/

TBCameraInstance *baGetCurrentCameraInstance();


/*	--------------------------------------------------------------------------------
	Function : baDeleteCameraInstance
	Purpose : delete a camera instance
	Parameters : ptr to camera instance or NULL to delete all cameras
	Returns : 
	Info : 
*/

void baDeleteCameraInstance(TBCameraInstance *camera);


/*	--------------------------------------------------------------------------------
	Function : baCreateCameraInstance
	Purpose : create a new camera instance
	Parameters : ptr to instance to fill or NULL for dynamic, flags, mode/parameters...
	Returns : ptr to new camera or NULL for failure
	Info : 
*/

TBCameraInstance *baCreateCameraInstance(TBCameraInstance *camera, const uint32 flags, ...);


/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstanceSource
	Purpose : set the source properties for a camera
	Parameters : ptr to camera, parameters
	Returns : 
	Info : 
*/

void baSetCameraInstanceSource(TBCameraInstance *camera, ...);


/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstanceTarget
	Purpose : set the target properties for a camera
	Parameters : ptr to camera, parameters
	Returns : 
	Info : 
*/

void baSetCameraInstanceTarget(TBCameraInstance *camera, ...);


/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstanceView
	Purpose : set the view info for a camera instance
	Parameters : camera, xFov, yFov, nearClip, farClip, flags (see BCAMERAINFO_)
	Returns : 
	Info : 
*/

void baSetCameraInstanceView(TBCameraInstance *camera, const float xFov, const float yFov, const float nearClip, const float farClip,
																												const uint32 flags);

#endif		// __BACTOR_CAMERA_H__
