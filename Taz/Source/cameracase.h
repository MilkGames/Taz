#ifndef __H_CAMERACASE
#define __H_CAMERACASE

#include "LinkedList.h"						// PP: linked-list class, used by Keep-On-Screen camera case type

#define SUSPENDFLAG_TAZ (1<<0)
#define SUSPENDFLAG_ENEMIES (1<<1)
#define SUSPENDFLAG_WORLD (1<<2)

#define CAMERACASEFLAG_SMOOTH				(1<<0)
#define CAMERACASE_FIXEDDISTANCEFROMTAZ		(1<<1)
#define CAMERACASE_EXITCUTSCENE				(1<<2)		// TP: load exit cutscene while playing camera case, then play exit
#define CAMERACASE_TRANSITIONCONTROL		(1<<3)		// PP: if this flag is high: when the camera case starts, the previous camera's angle will be used briefly by ConvertFromCameraToGlobalRotation
#define CAMERACASE_CAMERAPOSSNAP			(1<<4)
#define CAMERACASE_CAMERALOOKATSNAP			(1<<5)
#define CAMERACASE_CAMERAPOSSNAPBACK		(1<<6)
#define CAMERACASE_CAMERALOOKATSNAPBACK		(1<<7)

				// TP: Added these to maintain compatibility with existing cutscenes and the various default modes built in, what a pisser

#define CAMERACASE_CAMERAPOSSMOOTH			(1<<8)
#define CAMERACASE_CAMERALOOKATSMOOTH			(1<<9)
#define CAMERACASE_CAMERAPOSSMOOTHBACK		(1<<10)
#define CAMERACASE_CAMERALOOKATSMOOTHBACK		(1<<11)


enum{
	CAMERACASETYPE_FIXEDPOINT,		// camera and lookat lock to specific objects
	CAMERACASETYPE_ROTATEAROUND,	// camera rotate around focal point
	CAMERACASETYPE_PAN,				// lookat object with camera at fixed or pan points
	CAMERACASETYPE_KEEPONSCREEN,	// PP: keeps a number of specified actor instances on-screen
};

typedef struct TAG_CAMERACASE
{
	struct TAG_CAMERACASE			*next,*prev;			// next and previous camera case
	uint32							caseName;				// crc name of camera case
	int32							type;					// type of camera case CAMERACASETYPE_
//16 bytes													
	TBVector						pan1,pan2;				// vector coords for pan cam
//32 bytes													
	uint32							cameraFollow;			// crc of object for camera to follow
	uint32							cameraNode;				// TP: crc of node for camera to follow
															
	uint32							lookAtFollow;			// crc of object for camera lookat to follow
	uint32							lookAtNode;			// node to look at

	float							caseTime;				// time camera case last for, 0 for infinite					
//48 bytes		
	float							time;					// current time camera case active for
	float							startDelay;				// TP: delay prior to starting the camera case
	float							radius;					// radius for rotate around mode
												
	float							angle;					// angle above horizontal
	CAMERAINFO						*camera;				// camera for case
//64 bytes	

	// PP: data specific to particular camera case types...
	// PP: *** IMPORTANT: struct instances pointed to by this union will not have their destructors called on shutdown! ***
	union
	{
		// PP:						________________
		void*						typeSpecificData;		// PP: dummy pointer representing all the different type-specific pointers
		// PP:						¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
		struct TAG_KOSCAMDATA*		KOScamData;				// PP: data specific to the KEEPONSCREEN camera case type
	};

	uint32							cameraDest;				// TP: Camera destination used by pan camera
												
	char							suspend;				// Flags specifying what items to suspend
	int								flags;					// Other miscelaneous flags
	struct	TAG_TRIGGERINFO			*onStart;				// TP: messages to be sent as cameracase starts
	struct	TAG_TRIGGERINFO			*onEnd;					// TP: messages to be sent as cameracase ends
//80 bytes
	uint32							lockTazCRC;				// TP: crc of object to position taz at and hold until cameracase has finished


} CAMERACASE;

typedef struct TAG_CAMERACASELIST
{
	CAMERACASE		head;
	int				numEntries;
} CAMERACASELIST;

extern	float	defaultPosSmooth,defaultLookatSmooth;

/* --------------------------------------------------------------------------------
   Function : InitCameraCaseList
   Purpose : intialise the camera case list
   Parameters : 
   Returns : 
   Info : 
*/
void InitCameraCaseList(void);

/* --------------------------------------------------------------------------------
   Function : CreateCameraCase
   Purpose : create a camera case structure and initialise
   Parameters : name, type
   Returns : cameracase 
   Info : 
*/
CAMERACASE *CreateCameraCase(char *name,int32 type);

/* --------------------------------------------------------------------------------
   Function : CameraCasePan
   Purpose : Create a pan camera
   Parameters : cameraCase, lookat,lookat node, start pos, end pos, time
   Returns : 
   Info : 
*/
void CameraCasePan(CAMERACASE *cameraCase,char *lookAt,char *lookAtNode,TBVector pan1,TBVector pan2,float time);

/* --------------------------------------------------------------------------------
   Function : CameraCasePan
   Purpose : Create a pan camera
   Parameters : cameraCase, lookat,lookat node, start pos, end pos, time
   Returns : 
   Info : 
*/
void CameraCasePan(CAMERACASE *cameraCase,uint32 lookAt,uint32 lookAtNode,uint32 pan1,uint32 pan2,float time);

/* --------------------------------------------------------------------------------
   Function : CameraCaseFixedPoint
   Purpose : Create a fixed point camera
   Parameters : cameraCase, cameraFollow, lookAtFollow
   Returns : 
   Info : 
*/
void CameraCaseFixedPoint(CAMERACASE *cameraCase,uint32 cameraFollow,uint32 lookAtFollow,float time, uint32 lookAtNode = NULL, uint32 cameraNode=NULL);

/* --------------------------------------------------------------------------------
   Function : CameraCaseFixedPoint
   Purpose : Create a fixed point camera
   Parameters : cameraCase, cameraFollow, lookAtFollow
   Returns : 
   Info : 
*/
void CameraCaseFixedPoint(CAMERACASE *cameraCase,char *cameraFollow,char *lookAtFollow,float time);

/* --------------------------------------------------------------------------------
   Function : CameraCaseRotateAround
   Purpose : Create a rotate around camera case
   Parameters : cameraCase, lookAtFollow, radius
   Returns : 
   Info : 
*/
void CameraCaseRotateAround(CAMERACASE *cameraCase,char *lookAtFollow,float radius,float angle,char flags);

/* --------------------------------------------------------------------------------
   Function : CameraCaseRotateAround
   Purpose : Create a rotate around camera case
   Parameters : cameraCase, lookAtFollow, radius
   Returns : 
   Info : 
*/
void CameraCaseRotateAround(CAMERACASE *cameraCase,uint32 lookAtFollow,float radius,float angle,char flags);

/* --------------------------------------------------------------------------------
   Function : CameraCaseKeepOnScreen
   Purpose : Create a camera that keeps a list of actor instances on-screen
   Parameters : cameraCase, camera angle
   Returns : 
   Info : 
*/
void CameraCaseKeepOnScreen(CAMERACASE* const cameraCase, const TBVector angle);

/* --------------------------------------------------------------------------------
   Function : CameraCaseAttribs
   Purpose : Set the camera case attribs
   Parameters : cameraCase, suspendflags
   Returns : 
   Info : 
*/
void CameraCaseAttribs(CAMERACASE *cameraCase, uint32 suspend, uint32 flags);

/*  --------------------------------------------------------------------------------
	Function	: CameraCaseStartDelay
	Purpose		: Sets the delay prior to the cameracase starting
	Parameters	: cameracase, time in seconds
	Returns		: 
	Info		: 
*/
void CameraCaseStartDelay(CAMERACASE *cameraCase, float time);

/* --------------------------------------------------------------------------------
   Function : DeleteCameraCase
   Purpose : remove and delete a camera case
   Parameters : cameraCase
   Returns : 
   Info : 
*/
void DeleteCameraCase(CAMERACASE *cameraCase);

/* --------------------------------------------------------------------------------
   Function : DeleteAllCameraCases
   Purpose : remove and delete all camera cases
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteAllCameraCases(void);

/* --------------------------------------------------------------------------------
   Function : FindCameraCase
   Purpose : locate a camera case in the cameraCaseList
   Parameters : name
   Returns : CAMERACASE
   Info : 
*/
CAMERACASE *FindCameraCase(char *name);

/* --------------------------------------------------------------------------------
   Function : FindCameraCase
   Purpose : locate a camera case in the cameraCaseList
   Parameters : name crc
   Returns : CAMERACASE
   Info : 
*/
CAMERACASE *FindCameraCase(uint32 name);

/* --------------------------------------------------------------------------------
   Function : ProcessCameraMessages
   Purpose : process any camera messages sent
   Parameters : message
   Returns : 
   Info : 
*/
void ProcessCameraMessages(char *message);

/* --------------------------------------------------------------------------------
   Function : StartCameraCase
   Purpose : set up a camera case
   Parameters : cameraCase
   Returns : TRUE/FALSE if case could start
   Info : 
*/
int StartCameraCase(CAMERACASE *cameraCase,CAMERAINFO *camera);

/* --------------------------------------------------------------------------------
   Function : StopCameraCase
   Purpose : end a camera case
   Parameters : camera
   Returns : 
   Info : 
*/
void StopCameraCase(CAMERAINFO *camera,CAMERACASE *cameraCase, bool newMessages = true);

/* --------------------------------------------------------------------------------
   Function : UpdateCameraCase
   Purpose : update any camera case info
   Parameters : cameraCase
   Returns : 
   Info : 
*/
void UpdateCameraCase(CAMERAINFO *camera,CAMERACASE *cameraCase);

/*  --------------------------------------------------------------------------------
	Function	: SendCameraCaseMessages
	Purpose		: Send any messages associated with a cameracase start and end
	Parameters	: cameracase
	Returns		: 
	Info		: 
*/
void SendCameraCaseMessages(CAMERAINFO *camera, CAMERACASE *cameraCase);

#endif