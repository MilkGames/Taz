#ifndef __H_FRONTENDCAMERA
#define __H_FRONTENDCAMERA

#include "frontendmenu.h"

#define FECAMPOS	(0)				// CMD: defines udes to reference
#define FELOOKATPOS	(1)				// CMD: camera positions
#define FE_CAMERATIME	(2.0f)		// CMD: time taken for the camera to move between points

enum ECameras
{
	FECAM_MAIN,
	FECAM_STARTGAME,
	FECAM_OPTIONS,
	FECAM_AUDIO,
	FECAM_TWOPLAYER,
	FECAM_EXTRAS,
	FECAM_RECORDS,
	FECAM_TWOPLAYER_TOURNAMENT,
	FECAM_TWOPLAYER_TIME,
	FECAM_TWOPLAYER_DESTRUCTION,
	FECAM_TWOPLAYER_RACE,
	FECAM_EXTRAS_CREDITS,
	FECAM_EXTRAS_GALLERY,
	FECAM_EXTRAS_SECRETS,
	
	FECAM_NUMOFCAMERAS,
};

enum ECamPoints
{
	FE_STARTPT,
	FE_PT1,
	FE_PT2,
	FE_PT3,
	FE_PT4,
	FE_PT5,
	FE_PT6,
	FE_PT7,
	FE_PT8,
	FE_ENDPT,						// CMD: number of points in camera curve

	FENUMPOINTS,
};

typedef struct TAG_FECAMSTORE
{
	struct TAG_ACTORINSTANCE	*actorInstance;
	uint32						position;
	uint32						lookAtInstanceCRC;
	uint32						iPad;
}FECAMSTORE, _FECAMSTORE BALIGN16;

typedef struct TAG_FRONTENDCAMERAINFO
{
	struct TAG_CAMERAINFO	*cameraInfo;				//ptr to player 1 structure camera[0]
	uint32					currentPoint;
	uint32					destinationMenu;
	uint32					currentMenu;

	float					time;
	int						iPad[3];

	TBVector				tempCamStart[2];
	TBVector				cameras[MENU_NUMSUBMENUS][2];					//static menu camera positions
	TBVector				controlPts[2];
}FRONTENDCAMERAINFO, _FRONTENDCAMERAINFO BALIGN16;

extern FECAMSTORE frontEndCamStore[MENU_NUMSUBMENUS];

/*	--------------------------------------------------------------------------------
	Function 	: StoreFrontEndCameraInfoFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseFrontEndCameraStore(void);

/*	--------------------------------------------------------------------------------
	Function 	: StoreFrontEndCameraInfoFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StoreFrontEndCameraInfoFromMap(struct TAG_ACTORINSTANCE *actorInstance, int32 position, uint32 lookAtInstance);

/*	--------------------------------------------------------------------------------
	Function 	: FreeFrontEndCameraStore
	Purpose 	: frees memory used by front end camera store
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEndCameraStore(void);

/*	--------------------------------------------------------------------------------
	Function 	: AcceptFrontEndCamerasFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void AcceptFrontEndCamerasFromStore(struct TAG_ACTORINSTANCE *actorInstance, int32 position, uint32 lookAtInstance);

/*	--------------------------------------------------------------------------------
	Function 	: SetFrontEndCameraDestination
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void SetFrontEndCameraDestination(ECameras currentCamera, ECameras destinationCamera);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndCamera
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndCamera(void);

#endif