#ifndef __H_POSTCARD
#define __H_POSTCARD

#include "camera.h"

typedef struct TAG_POSTCARD
{
	ACTORINSTANCE	*actorInstance;		// actor holding the postcard
	char			name[32];			//
	CAMERAINFO		*camera;			// camera to use
	CAMERAINFO		cameraSave;			// keep hold of old camera settings
	int				attachHandle;		// handle for the postcard object attached
	float			timeSinceCollected;	// time since collect postcard was called
	char			postcardCameraActive;	// is the postcard camera active
} POSTCARD;

/*	--------------------------------------------------------------------------------
	Function 	: CollectPostcard
	Purpose 	: Start the postcard mode, add postcard to character
	Parameters 	: character to hold postcard, postcard structure, camera to use, postcard object, node name to attach postcard to
	Returns 	: TRUE/FALSE
	Info 		: 
*/
int	CollectPostcard(ACTORINSTANCE *actorInstance, POSTCARD *postcard, CAMERAINFO *camera, char *postcardName, char *node);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePostcard
	Purpose 	: Perform an update on the current postcard
	Parameters 	: POSTCARD
	Returns 	: TRUE/FALSE
	Info 		:
*/
void UpdatePostcard(POSTCARD *postcard);

/*	--------------------------------------------------------------------------------
	Function 	: StartPostcardCamera
	Purpose 	: Start the postcard camera mode
	Parameters 	: POSTCARD, eye node name
	Returns 	: 
	Info 		: 
*/
void StartPostcardCamera(POSTCARD *postcard,char *eyeNodeName);

/*	--------------------------------------------------------------------------------
	Function 	: EndPostcardCamera
	Purpose 	: End the postcard camera mode
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		: 
*/
void EndPostcardCamera(POSTCARD *postcard);

/*	--------------------------------------------------------------------------------
	Function 	: RemovePostcard
	Purpose 	: Finished with postcard so remove and change mode
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		:
*/
void RemovePostcard(POSTCARD *postcard);

/*	--------------------------------------------------------------------------------
	Function 	: GetPostcardTime
	Purpose 	: returns how long postcard has been active
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		:
*/
float GetPostcardTime(POSTCARD *postcard);

#endif