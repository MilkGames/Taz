// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : postcard.cpp
//   Purpose : functions for Taz viewing postcards
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "attach.h"
#include "maths.h"
#include "thoughtbubble.h"
#include "characters.h"
#include "main.h"
#include "postcard.h"


/*	--------------------------------------------------------------------------------
	Function 	: CollectPostcard
	Purpose 	: Start the postcard mode, add postcard to character
	Parameters 	: character to hold postcard, postcard structure, camera to use, postcard object, node name to attach postcard to
	Returns 	: TRUE/FALSE
	Info 		: 
*/
int	CollectPostcard(ACTORINSTANCE *actorInstance, POSTCARD *postcard, CAMERAINFO *camera, char *postcardName, char *node)
{
	TBVector	objectPos;
	TBVector	objectScale;
	TBQuaternion	tempQuat;

	postcard->actorInstance = actorInstance;
	postcard->camera = camera;
	postcard->postcardCameraActive = FALSE;

	objectPos[X] = 0.0f;
	objectPos[Y] = 0.0f;
	objectPos[Z] = 0.0f;
	objectPos[W] = 1.0f;
	objectScale[X] = 1.0f;
	objectScale[Y] = 1.0f;
	objectScale[Z] = 1.0f;
	objectScale[W] = 1.0f;
	tempQuat[X] = 0.0f;
	tempQuat[Y] = 0.0f;
	tempQuat[Z] = 0.0f;
	tempQuat[W] = 1.0f;

//	bmRotationToQuat(objectQuat,tempQuat);

	postcard->attachHandle = 
		AttachObjectToCharacter(actorInstance, postcardName, node, objectPos, objectScale, tempQuat, 0);

	if (postcard->attachHandle!=-1)
	{
		SetGameSubState(&gameStatus,GAMESUBSTATE_POSTCARD);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePostcard
	Purpose 	: Perform an update on the current postcard
	Parameters 	: POSTCARD
	Returns 	: TRUE/FALSE
	Info 		:
*/
void UpdatePostcard(POSTCARD *postcard)
{
//	TBVector pos;

	postcard->timeSinceCollected += 1.0f/fps;

	if (postcard->postcardCameraActive)
	{
//		GetAttachedObjectsWorldPosition(postcard->actorInstance,postcard->attachHandle,pos);
//		SetCameraLookAtDestination(postcard->camera,pos);
	}
}

void PostCardText(TBMatrix objectMatrix, /*PP:*/ void* const context)
{
	char  *buf = "Dear Taz @(n)Find all the mailboxes on this level to escape.@(n)";
	ushort string16[256];
	TBFontFormatting	format;

	format.x = -130;
	format.y = 60;
	format.height = 120;
	format.width = 260;
	format.horzFormat = EBFONTFORMATTING_LEFT;
	format.vertFormat = EBFONTFORMATTING_TOP;
	format.wrap = 1;
	format.lineSpace = 0;

	bkString8to16(string16, buf);
	bdPrintFont(standardFont,string16,strlen(buf),&format,0,0,0,127,NULL,NULL);
}

/*	--------------------------------------------------------------------------------
	Function 	: StartPostcardCamera
	Purpose 	: Start the postcard camera mode
	Parameters 	: POSTCARD, node name for eye position
	Returns 	: 
	Info 		: 
*/
void StartPostcardCamera(POSTCARD *postcard, char *eyeNodeName)
{
	TBVector tempVector;
	TBQuaternion quat;

//	SaveCameraSettings(&postcard->cameraSave,postcard->camera);
	// move camera in
//	SetFixedDistanceToLookAt(postcard->camera,0.0f);
//	DisableCameraAngleAboveHorizontal(postcard->camera);

//	bmVectorCopy(tempVector,postcard->actorInstance->actorInstance.position);
//	tempVector[Y] += METERTOUNIT(0.5f);
//	SetCameraPositionDestination(postcard->camera,tempVector);
//	SetCameraSmooth(postcard->camera,0.9f,0.9f);
//	SetCameraPosFollowActorInstance(postcard->camera, &postcard->actorInstance->actorInstance, eyeNodeName);

//	DisableCameraLookAtFollowActorInstance(postcard->camera);
//	SetCameraLookAtFollowActorInstance(postcard->camera,&postcard->actorInstance->actorInstance,"fingerr2b");

	SETVECTOR(tempVector,1.0f,0.0f,0.0f,RAD(-90));
	bmRotationToQuat(quat,tempVector);

	SETVECTOR(tempVector,METERTOUNIT(0.5f),METERTOUNIT(1.0f),0.0f,1.0f);

	CreateThoughtBubble(postcard->actorInstance->characterInfo->thoughtBubble,tempVector,METERTOUNIT(2.5f),METERTOUNIT(1.75),5,PostCardText);

	AddObjectToThoughtBubble(postcard->actorInstance->characterInfo->thoughtBubble,"postcard.obe",METERTOUNIT(0.6f),0,4.5f,quat);

	postcard->postcardCameraActive = TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: EndPostcardCamera
	Purpose 	: End the postcard camera mode
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		: 
*/
void EndPostcardCamera(POSTCARD *postcard)
{
//	RestoreCameraSettings(postcard->camera,&postcard->cameraSave);

	CloseThoughtBubble(postcard->actorInstance->characterInfo->thoughtBubble);

	postcard->postcardCameraActive = FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: RemovePostcard
	Purpose 	: Finished with postcard so remove and change mode
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		:
*/
void RemovePostcard(POSTCARD *postcard)
{
	RemoveObjectFromCharacter(postcard->actorInstance,postcard->attachHandle);
	SetGameSubState(&gameStatus,GAMESUBSTATE_NORMAL);
}

/*	--------------------------------------------------------------------------------
	Function 	: GetPostcardTime
	Purpose 	: returns how long postcard has been active
	Parameters 	: POSTCARD
	Returns 	: 
	Info 		:
*/
float GetPostcardTime(POSTCARD *postcard)
{
	return postcard->timeSinceCollected;
}