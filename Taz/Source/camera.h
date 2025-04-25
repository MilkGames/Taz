#ifndef __H_CAMERA
#define __H_CAMERA

#include "actors.h"

// PP: Camera flags
enum ECameraFlags
{
	CAMERA_ANGLEOVERRIDEY		=1<<0,
	CAMERA_FIRSTPERSONMODE		=1<<1,
	CAMERA_ROTATEBEHIND			=1<<2,
	CAMERA_QUAKE				=1<<3,
	CAMERA_QUAKEFADE			=1<<4,
	CAMERA_QUAKEBANG			=1<<5,
	CAMERA_CHASECAM				=1<<6,
	CAMERA_FOLLOWBEHIND			=1<<7,
	CAMERA_CATCHUP				=1<<8,
	CAMERA_NOCOLLISION			=1<<9,
	CAMERA_BIGFALL				=1<<10,
	CAMERA_LOOKAROUND			=1<<11,
	CAMERA_PADCONTROL			=1<<12,				// PP: camera can be rotated using the joypad
	CAMERA_RUMBLEFADE			=1<<13,				// PP: smooth fading shake enabled for the camera
	CAMERA_BLINKEYES			=1<<14,				// TP: Draw the taz blinking eyes
	CAMERA_ROPECONTROL			=1<<15,				// CMD: Taz Playing with his monkeys.
	CAMERA_SMARTCAM				=1<<16,				// TP: on by default this enables the kinda smart camera mode
	CAMERA_CUTSCENE				=1<<17,				// TP: tweeks a few features to make the cutscenes work better

	// PP: flags with which cameras get created by default
	CAMERA_DEFAULT_FLAGS		=CAMERA_PADCONTROL|CAMERA_SMARTCAM
};

#define CAMERA_RFREACHEDPOS		(1<<0)

#include "fade.h"


typedef struct TAG_CAMERAINFO
{
	TBVector		pos;							// current position of camera
//
	TBVector		lookAt;							// current coords to look at
//	
	TBVector		destinationPos;					// destination for the cameras position
//	
	TBVector		destinationLookAt;				// destination for the cameras look at position
//	
	TBVector		actorInstanceLookAtOffset;		// offset from the centre of the actor instance to the point at which the camera looks at
//
	TBVector		resetPoint;						// reset point associated with last mailbox
//
	float			fixedDistanceToLookAt;			// value for distance to maintain between camera and look at, if 0 then no fixed distance
	float			angleAboveHorizontal;			// optimum angle above horizontal, CAMERA_ANGLEOVERRIDEY must be set
	float			posSmooth;
	float			lookAtSmooth;			// values for smoothing camera movement 0(too smooth to move) to 1(no smooth)
//					
	float			rotRadsPerSecond;				// Rad's to rotate in one second for rotate behind mode
	uint32			flags;							// camera flags
	uint32			readFlags;						// CPW: flags indicating stuff (see above)
	float			tilt;
	float			tiltOffset;						// offset value for tilt
//					
	TBVector		posOffset;						// offset for camera position (e.g. used by quakeCam)
//					
	TBVector		lookAtOffset;					// offset for camera look at position
//
	TBActorInstance		*actorInstanceLookAt;			// actor instance to look at or NULL (in first person mode this indicates the actor for the camera to use)
	TBActorNodeInstance	*actorInstanceNodeLookAt;		// node on the actor to look at or NULL if none
	TBActorInstance		*actorInstancePos;				// actor for CAMERA not LOOKAT to follow
	TBActorNodeInstance	*actorInstanceNodePos;			// node on the actor for the camera to follow or NULL if none
//
	ACTORINSTANCE		*ourActorInstanceLookAt;		// CPW: we need this higher level one for some purposes
//
	struct TAG_ACTORINSTANCE		collisionActor;		// actor for use in collision tests
//
	struct TAG_CAMERACASE	*cameraCase;				// temporary camera override, for camera cases etc.
	uint32				crcQueuedCase;					// TP: crc of queued camera case
	float				queueDelay;						// TP: delay before queued case is played

	float				quakeOffsetLevel;				//used to keep track of quake level
	float				tiltOffsetLevel;				//used to keep track of tilt
	float				*chaseCamVel;					// pointer to velocity for chase cam

	float				defaultAngleAboveHorizontal;
	float				defaultFixedDistance;
	FADE				fade;							// TP: fade information about camera
	struct
	{
		TBVector			cameraAngles;					// TP: current angles X,Y and Z of the camera
		TBVector			cameraPos;
		TBVector			cameraLookat;
	}cameraState;										// TP: This contains the exact camera state that babel has

	struct
	{
		int currentIndex;
		TBVector	curDest;
	}followBehind;

	struct
	{
		float stopTime;
		TBVector startPos;
	}bigfall;

	// PP: data for the 'rumble' (smooth shake) effects
	struct
	{
		TBVector	angOffset;					// PP: angle offset
		TBVector	angOffsetVelocity;			// PP: velocity of angle offset
		TBVector	lookAtOffsetVelocity;		// PP: velocity of look-at offset
		TBVector	posOffsetVelocity;			// PP: velocity of position offset

	}rumbleData;

	struct TAG_SCREENSMASHINFO	*smashInfo;		// CMD: stuff to do with smashed screed overlay

}CAMERAINFO, _CAMERAINFO BALIGN16;

#if 1
	extern float CAMERADIST;
	extern float CAMERAANGLE;
#else
	#define CAMERADIST	(500.0f)
	#define CAMERAANGLE (0.3f)
#endif
// CPW: #define CAMERAANGLE (0.0f)

extern CAMERAINFO	camera[];				// TP: One camera per screen


/*	--------------------------------------------------------------------------------
	Function 	: SimplifyCamera
	Purpose 	: take all the fancy stuff off a camera, so you can control it simply by its pos & lookat
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: These bloomin' fancy cameras!  They've got mind of their own!
*/
void SimplifyCamera(CAMERAINFO* const cameraInfo);


/* --------------------------------------------------------------------------------
   Function : InitCamera
   Purpose : Intialise camera structure
   Parameters : CAMERAINFO, origin, lookAt
   Returns : 
   Info : 
*/
void InitCamera(CAMERAINFO *cameraInfo, TBVector origin, TBVector lookAt,bool initStructure = true);

/* --------------------------------------------------------------------------------
   Function : DestroyCamera
   Purpose : destroy camera structure
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DestroyCamera(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : CreateCameraCollSphere
   Purpose : Create a collision sphere for the camera
   Parameters : CAMERAINFO, sphere offset, radius, maxSpeed(not currently supported)
   Returns : 
   Info : 
*/
void CreateCameraCollSphere(CAMERAINFO *cameraInfo, TBVector offset, float radius, float maxSpeed);

/* --------------------------------------------------------------------------------
   Function : UpdateCamera
   Purpose : Update camera info, move camera etc.
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void UpdateCamera(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : SendCamera
   Purpose : Send the camera information to babel
   Parameters : (opt/NULL=copy of last camera sent) CAMERAINFO
   Returns : 
   Info : 
*/
void SendCamera(CAMERAINFO *cameraInfo=NULL);

/* --------------------------------------------------------------------------------
   Function : SetCameraPosition
   Purpose : Sets the current camera position and camera destination
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraPosition(CAMERAINFO *cameraInfo, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : SetCameraPositionOffset
   Purpose : Sets the current position offset
   Parameters : CAMERAINFO, (opt/NULL=no offset)TBVector position
   Returns : 
   Info : 
*/

void SetCameraPositionOffset(CAMERAINFO *cameraInfo, TBVector offset=NULL);

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtPosition
   Purpose : Sets the current look at position and look at destination
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraLookAtPosition(CAMERAINFO *cameraInfo, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtOffset
   Purpose : Sets the current look at position offset
   Parameters : CAMERAINFO, (opt/NULL=no offset)TBVector position
   Returns : 
   Info : 
*/
void SetCameraLookAtOffset(CAMERAINFO *cameraInfo, TBVector pos=NULL);

/* --------------------------------------------------------------------------------
   Function : SetCameraPositionDestination
   Purpose : Sets the destination for the camera position to move to
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraPositionDestination(CAMERAINFO *cameraInfo, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtDestination
   Purpose : Sets the destination for the camera look at to move to
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraLookAtDestination(CAMERAINFO *cameraInfo, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtFollowActorInstance
   Purpose : Sets the destination for the camera look at to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo, ACTORINSTANCE *ourActorInstance, TBActorInstance *actorInstance, char *node);


/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtFollowActorInstance
   Purpose : Sets the destination for the camera look at to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo, ACTORINSTANCE *ourActorInstance, TBActorInstance *actorInstance, TBActorNodeInstance *node);

/* --------------------------------------------------------------------------------
   Function : DisableCameraLookAtFollowActorInstance
   Purpose : disables the look at follow instance mode
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo);


/* --------------------------------------------------------------------------------
   Function : SetCameraPosFollowActorInstance
   Purpose : Sets the destination for the camera pos to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraPosFollowActorInstance(CAMERAINFO *cameraInfo, TBActorInstance *actorInstance, char *node);

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtActorInstanceOffset
   Purpose : Sets the offset from the center of the actor instance to point the camera at
   Parameters : CAMERAINFO, TBVector offset
   Returns : 
   Info : 
*/
void SetCameraLookAtActorInstanceOffset(CAMERAINFO *cameraInfo, TBVector offset);

/* --------------------------------------------------------------------------------
   Function : SetDefaultCameraAngleAboveHorizontal
   Purpose : Sets the default angle above horizontal for the camera
   Parameters : CAMERAINFO, angle in rads above horizontal
   Returns : 
   Info : 
*/
void SetDefaultCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo, float angle);

/* --------------------------------------------------------------------------------
   Function : RotateCameraAroundLookAt
   Purpose : Rotates the camera in the Y and X rotations around the lookat point
   Parameters : CAMERAINFO, TBActorInstance
   Returns : 
   Info : 
*/
void RotateCameraAroundLookAt(CAMERAINFO *cameraInfo, float yAngle,float xAngle);

/* --------------------------------------------------------------------------------
   Function : ConvertFromCameraToGlobalRotation
   Purpose : Converts an angle from camera relative to global
   Parameters : CAMERAINFO, angle
   Returns : global angle
   Info : 
*/
float ConvertFromCameraToGlobalRotation(CAMERAINFO *cameraInfo,float angle);

/* --------------------------------------------------------------------------------
   Function : SetFixedDistanceToLookAt
   Purpose : Sets the maximum and minimum distance between the camera and the lookat
   Parameters : CAMERAINFO, float distance
   Returns : 
   Info : 
*/
void SetFixedDistanceToLookAt(CAMERAINFO *cameraInfo, float distance);

/* --------------------------------------------------------------------------------
   Function : EnableCameraAngleAboveHorizontal
   Purpose : Sets the optimum angle above horizontal for the camera
   Parameters : CAMERAINFO, angle in rads above horizontal
   Returns : 
   Info : 
*/
void EnableCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo, float angle);

/* --------------------------------------------------------------------------------
   Function : DisableCameraAngleAboveHorizontal
   Purpose : Disables the angle override for camera height
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : SetCameraSmooth
   Purpose : Set camera smooth values for the camera and lookat points
   Parameters : CAMERAINFO, float posSmooth, float lookAtSmooth
   Returns : 
   Info : Controls how fast the camera and lookAt positions move to their destination positions
			0 - no move, 1 - move entire distance in one update
*/
void SetCameraSmooth(CAMERAINFO *cameraInfo, float posSmooth, float lookAtSmooth);

/* --------------------------------------------------------------------------------
   Function : SaveCameraSettings
   Purpose : Save all camera settings except current camera pos and current camera lookAt pos
   Parameters : dest,source
   Returns : 
   Info : 
*/
void SaveCameraSettings(CAMERAINFO *dest, CAMERAINFO *source);

/* --------------------------------------------------------------------------------
   Function : RestoreCameraSettings
   Purpose : restore all camera settings except pos and lookAt pos
   Parameters : dest,source
   Returns : 
   Info : 
*/
void RestoreCameraSettings(CAMERAINFO *dest, CAMERAINFO *source, bool restorePos=false, bool restoreFade=true);

/* --------------------------------------------------------------------------------
   Function : EnableCameraRotateBehind
   Purpose : Sets the camera to rotate behind the actorlookat, (only works with SetCameraLookAtFollowActorInstance)
   Parameters : CAMERAINFO, max rads to rotate in one second
   Returns : 
   Info : 
*/
void EnableCameraRotateBehind(CAMERAINFO *cameraInfo, float radsPerSecond);

/* --------------------------------------------------------------------------------
   Function : DisableCameraRotateBehind
   Purpose : Disable the rotate behind code
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraRotateBehind(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : EnableCameraFollowBehind
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void EnableCameraFollowBehind(CAMERAINFO *cameraInfo, ACTORINSTANCE *);

/* --------------------------------------------------------------------------------
   Function : DisableCameraFollowBehind
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DisableCameraFollowBehind(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : EnableCameraCatchup
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void EnableCameraCatchup(CAMERAINFO *cameraInfo, ACTORINSTANCE *);

/* --------------------------------------------------------------------------------
   Function : EnableCameraBigFall
   Purpose : Sets the camera to follow a big fall
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void EnableCameraBigFall(CAMERAINFO *cameraInfo, ACTORINSTANCE *pActorInstance);

/* --------------------------------------------------------------------------------
   Function : DisableCameraBigFall
   Purpose : 
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraBigFall(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : DisableCameraCatchup
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DisableCameraCatchup(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetCameraCollision(CAMERAINFO *cameraInfo, int collide);

/* --------------------------------------------------------------------------------
   Function : EnableCameraChaseCam
   Purpose : Sets the camera to rotate behind the actor velocity, (only works with SetCameraLookAtFollowActorInstance)
   Parameters : CAMERAINFO, max rads to rotate in one second
   Returns : 
   Info : 
*/

void EnableCameraChaseCam(CAMERAINFO *cameraInfo, float radsPerSecond, TBVector vel);

/* --------------------------------------------------------------------------------
   Function : DisableCameraChaseCam
   Purpose : Disable the chase came code
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraChaseCam(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : SetCameraCase
   Purpose : Set the current camera override
   Parameters : dest,source
   Returns : 
   Info : the override camera will take over for standard cam although both will be updated
*/
void SetCameraCase(CAMERAINFO *camera,struct TAG_CAMERACASE *cameraCase);

/*  --------------------------------------------------------------------------------
	Function	: RespondToCameraControl
	Purpose		: Adjust the camera based on the control input
	Parameters	: player
	Returns		: 
	Info		: 
*/
void RespondToCameraControl(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: ResetCamera
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ResetCamera(CAMERAINFO *pCameraInfo);

/* --------------------------------------------------------------------------------
   Function : SetCameraPosFollowActorInstance
   Purpose : Sets the destination for the camera pos to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraPosFollowActorInstanceNode(CAMERAINFO *cameraInfo, TBActorInstance *actorInstance, TBActorNodeInstance *node);



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FLY CAM
										
							Handy debug camera that can be flown freely around the environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


extern CAMERAINFO			flyCam;					// PP: the debug camera that can be flown around freely
extern bool					flyCamActive;			// PP: is the fly cam active


/*	--------------------------------------------------------------------------------
	Function 	: initFlyCam
	Purpose 	: init fly cam
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initFlyCam(void);


/*	--------------------------------------------------------------------------------
	Function 	: setFlyCam
	Purpose 	: set the fly cam on/off
	Parameters 	: true:on, false:off
	Returns 	: 
	Info 		: 
*/
void setFlyCam(const bool on);


/*	--------------------------------------------------------------------------------
	Function 	: toggleFlyCam
	Purpose 	: toggle the fly cam on/off
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void toggleFlyCam(void);


/*	--------------------------------------------------------------------------------
	Function 	: updateFlyCam
	Purpose 	: update the fly cam
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void updateFlyCam(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FiRST-PERSON MODE

										Basically attaches the camera to Taz's heed.
										
								This should be interesting, especially in stereoscopic mode.
				It'd be cool to award this and the stereoscopic mode together as cheats when you complete the game.
									It lets you see the whole game from Taz's perspective.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


extern bool			firstPersonMode;
extern CAMERAINFO	firstPersonCam;


/*	--------------------------------------------------------------------------------
	Function 	: updateFirstPersonMode
	Purpose 	: update for first-person mode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void updateFirstPersonMode(void);


/*	--------------------------------------------------------------------------------
	Function 	: drawFirstPersonMode
	Purpose 	: draw for first-person mode
	Parameters 	: 
	Returns 	: 
	Info 		: probably just for early debug?  dunno yet.
*/
void drawFirstPersonMode(void);


/*	--------------------------------------------------------------------------------
	Function 	: updateFirstPersonMode
	Purpose 	: switch first-person mode on/off
	Parameters 	: true=on/ false=off
	Returns 	: 
	Info 		: 
*/
void setFirstPersonMode(const bool on);


/*	--------------------------------------------------------------------------------
	Function 	: toggleFirstPersonMode
	Purpose 	: toggle first-person mode on/off
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void toggleFirstPersonMode(void);


/*  --------------------------------------------------------------------------------
	Function	: ProcessCameraEnterLookAround
	Purpose		: Called each frame while the camera enters the lookaround mode
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/

void ProcessCameraEnterLookAround(struct TAG_PLAYER *player);


/*  --------------------------------------------------------------------------------
	Function	: ProcessCameraLookAround
	Purpose		: Called each frame while the lookaround mode is active
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/

void ProcessCameraLookAround(struct TAG_PLAYER *player);


#endif
