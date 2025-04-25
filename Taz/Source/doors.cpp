// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : doors.cpp
//   Purpose : provides door functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "status.h"
#include "collisioncache.h"
#include "scene.h"
#include "main.h"
#include "physics.h"
#include "quakecam.h"
#include "doors.h"
#include "sfx.h"
#include "events.h"
#include "animation.h"
#include "genericai.h"
#include "shadow.h"
#include "characters.h"
#include "cameracase.h"
#include "paths.h"

/* --------------------------------------------------------------------------------
   Function : CreateSwingingDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

float	xDoorEnter;

BOOK	*XDoorBook = 0;

int CreateDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type)
{
	switch(type)
	{
	case DOORTYPE_REVOLVING:
		CreateRevolvingDoor(actorInstance, springyness, stiffness, type);
		return TRUE;
	case DOORTYPE_SPRINGY:
		CreateSpringyDoor(actorInstance, springyness, stiffness, type);
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CreateSwingingDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateRevolvingDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type)
{
	actorInstance->special = SPECIALTYPE_DOOR;
	if((actorInstance->doorInfo = CreateDoorInfo()) == NULL)
	{
		return FALSE;
	}
	bmQuatCopy(actorInstance->doorInfo->defaultOrientation, actorInstance->actorInstance.orientation);
	actorInstance->doorInfo->springyness = springyness;
	actorInstance->doorInfo->stiffness = stiffness;
	actorInstance->doorInfo->update = FALSE;
	actorInstance->doorInfo->collision = FALSE;
	actorInstance->doorInfo->state = NULL;
	actorInstance->doorInfo->timeSinceLastCollision = 0.0f;
	actorInstance->doorInfo->type = type;

	AddActorToCollisionCache(&collisionCache, actorInstance, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(actorInstance, RespondToRevolvingDoorCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateSpringyDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSpringyDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type)
{
	actorInstance->special = SPECIALTYPE_DOOR;
	if((actorInstance->doorInfo = CreateDoorInfo()) == NULL)
	{
		return FALSE;
	}
	bmQuatCopy(actorInstance->doorInfo->defaultOrientation, actorInstance->actorInstance.orientation);
	actorInstance->doorInfo->springyness = springyness;
	actorInstance->doorInfo->stiffness = stiffness;
	actorInstance->doorInfo->type = type;
	actorInstance->doorInfo->update = FALSE;
	actorInstance->doorInfo->collision = FALSE;
	actorInstance->doorInfo->state = NULL;
	actorInstance->doorInfo->timeSinceLastCollision = 0.0f;
	actorInstance->doorInfo->oscillationTime = 0.0f;
	actorInstance->doorInfo->currentAngle = 0.0f;
	actorInstance->doorInfo->currentAmplitude = RAD(90.0f);

	AddActorToCollisionCache(&collisionCache, actorInstance, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(actorInstance, RespondToSpringyDoorCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateDoorInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

DOORINFO *CreateDoorInfo(void)
{
	DOORINFO *tempInfo;

	tempInfo = (DOORINFO*)ZALLOC(sizeof(DOORINFO));
	if(tempInfo)
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("*** WARNING *** door info structure could not be created\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateDoors
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateDoors(ACTORINSTANCE *ptr)
{
	if(ptr->doorInfo)
	{
		if(ptr->doorInfo->state != DOOR_NO_UPDATE)
		{
			switch(ptr->doorInfo->type)
			{
			case DOORTYPE_REVOLVING:
				UpdateRevolvingDoorOrientation(ptr);
				break;
			case DOORTYPE_SPRINGY:
				UpdateSpringyDoorOrientation(ptr);
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateDoorOrientation
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRevolvingDoorOrientation(ACTORINSTANCE *door)
{
	TBVector		yAxis;
//	TBVector		defaultRotation, currentRotation;
	TBQuaternion	tempQuat;
//	float			angleDifference;
//	float			tempAngularVelocity;
//	float			dampingVelocity;

	if(door->doorInfo->collision)
	{
		SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, 1.0f);
		//calculate appropriate angle to turn door
		switch(door->doorInfo->rotation)
		{
		case DOOR_ROTATION_CLOCKWISE:
			yAxis[3] = -(door->doorInfo->stiffness / fps);
			break;
		case DOOR_ROTATION_ANTICLOCKWISE:
			yAxis[3] = door->doorInfo->stiffness / fps;
			break;
		}

		if(door->doorInfo->state == DOOR_SPIN_UPDATE)
		{
			yAxis[3] *= 10.0f;
		}

		bmRotationToQuat(tempQuat, yAxis);
		bmQuatMultiply(door->actorInstance.orientation, tempQuat, door->actorInstance.orientation);

		door->doorInfo->update = TRUE;
		door->doorInfo->collision = FALSE;
	}
	else
	{
		if((door->doorInfo->update)&&((door->doorInfo->timeSinceLastCollision += 1.0f/fps) >= 0.3f))
		{
			bmQuatSlerp(door->actorInstance.orientation, door->actorInstance.orientation, door->doorInfo->defaultOrientation, 0.05f);
		/*	SETVECTOR(defaultRotation, 0.0f, 1.0f, 0.0f, 1.0f);
			SETVECTOR(currentRotation, 0.0f, 1.0f, 0.0f, 1.0f);

			//calculate default orientation angle
			bmQuatToRotation(defaultRotation, door->doorInfo->defaultOrientation);
			//calculate current orientation angle
			bmQuatToRotation(currentRotation, door->actorInstance.orientation);
			angleDifference = currentRotation[3] - defaultRotation[3];

			tempAngularVelocity = door->doorInfo->springyness * bmCos(angleDifference);
			tempAngularVelocity = CheckQuakeTiltForOpposites(angleDifference, tempAngularVelocity);

			dampingVelocity = CheckQuakeTiltForOpposites(tempAngularVelocity, door->doorInfo->stiffness);

			//add to current velocity
			door->doorInfo->angularVelocity += tempAngularVelocity;
			door->doorInfo->angularVelocity += dampingVelocity;
			//update position
			SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, (door->doorInfo->angularVelocity/fps));
			bmRotationToQuat(tempQuat, yAxis);
			bmQuatMultiply(door->actorInstance.orientation, tempQuat, door->actorInstance.orientation);
			*/
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateSpringyDoorOrientation
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSpringyDoorOrientation(ACTORINSTANCE *door)
{
	TBQuaternion	tempQuat;
	TBVector		yAxis;
	float			newAngle;
	float			freq = 1.0f;

 	if(door->doorInfo->collision)
	{
		newAngle = door->doorInfo->currentAmplitude*bmSin((door->doorInfo->oscillationTime += fTime)*freq);

		door->doorInfo->currentAmplitude -= RAD(30.0f)*bmCos(newAngle)*bmCos(newAngle)*fTime;

		if((door->doorInfo->currentAmplitude < RAD(3.0f))&&(newAngle < RAD(1.0f)))
		{
			door->doorInfo->oscillationTime = 0.0f;
			door->doorInfo->currentAmplitude = RAD(90.0f);
			door->doorInfo->collision = FALSE;
			newAngle = 0.0f;
		}
		if(door->doorInfo->rotation == DOOR_ROTATION_CLOCKWISE)
		{
			newAngle = CheckQuakeTiltForOpposites(newAngle, newAngle);
		}

		SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, newAngle);
		bmRotationToQuat(tempQuat, yAxis);
		bmQuatMultiply(door->actorInstance.orientation, tempQuat, door->doorInfo->defaultOrientation);
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToDoorCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToRevolvingDoorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *door)
{
	TBMatrix		tempMatrix;
	TBVector		collisionNormal, doorDirection;
	float			dotProduct;

	//calculate direction of collision
	bmVectorCopy(collisionNormal, collider->actorBehaviour->physics.lastAverageNormal);
	collisionNormal[1] = 0.0f;
	bmVectorNorm(collisionNormal, collisionNormal);

	//calculate current direction of door
	bmQuatToMatrix(tempMatrix, door->actorInstance.orientation);
	SETVECTOR(doorDirection, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(tempMatrix, doorDirection);
	bmVectorNorm(doorDirection, doorDirection);

	//check vectors agains one another
	dotProduct = bmVectorDot(collisionNormal, doorDirection);
	
	if(bmFAbs(dotProduct) >= 0.3f)
	{
		if(dotProduct > 0.0f)
		{
			door->doorInfo->rotation = DOOR_ROTATION_CLOCKWISE;
		}
		else
		{
			door->doorInfo->rotation = DOOR_ROTATION_ANTICLOCKWISE;
		}
		door->doorInfo->collision = TRUE;
		door->doorInfo->update = FALSE;
		door->doorInfo->timeSinceLastCollision = 0.0f;
	}

	//check for Taz state
	if(collider->actorStatus->currentState == STATE_SPIN)
	{
		door->doorInfo->state = DOOR_SPIN_UPDATE;
	}
	else
		if(bmVectorLen(collider->actorBehaviour->physics.velocity) < METERTOUNIT(0.1f))
		{
			door->doorInfo->state = DOOR_NO_UPDATE;
		}
		else
		{
			door->doorInfo->state = NULL;
		}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToDoorCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSpringyDoorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *door)
{
	TBMatrix		tempMatrix;
	TBVector		collisionNormal, doorDirection;
	float			dotProduct;

	if (door->doorInfo->collision==TRUE) return FALSE;

	//calculate direction of collision
	bmVectorCopy(collisionNormal, collider->actorBehaviour->physics.lastAverageNormal);
	collisionNormal[1] = 0.0f;
	bmVectorNorm(collisionNormal, collisionNormal);

	//calculate current direction of door
	bmQuatToMatrix(tempMatrix, door->actorInstance.orientation);
	SETVECTOR(doorDirection, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(tempMatrix, doorDirection);
	bmVectorNorm(doorDirection, doorDirection);

	//check vectors agains one another
	dotProduct = bmVectorDot(collisionNormal, doorDirection);
	
	if(bmFAbs(dotProduct) >= 0.3f)
	{
		if(dotProduct > 0.0f)
		{
			door->doorInfo->rotation = DOOR_ROTATION_CLOCKWISE;
		}
		else
		{
			door->doorInfo->rotation = DOOR_ROTATION_ANTICLOCKWISE;
		}
		door->doorInfo->collision = TRUE;
	}

	PlaySample("obj_mus_12a.wav",255,door->actorInstance.position);

	return FALSE;
}


/* TP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PLANET X DOOR

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void FaceDoor(ACTORINSTANCE *actorInstance, ACTORINSTANCE *door, float angle=180.0f)
{
	TBVector		tempVector;
	TBQuaternion	tempQuat;

	bmVectorSet(tempVector, 0.0f, 1.0f, 0.0f, RAD(angle));
	bmRotationToQuat(tempQuat, tempVector);
	bmQuatMultiply(actorInstance->actorInstance.orientation, tempQuat, door->actorInstance.orientation);
	bmQuatCopy(actorInstance->actorBehaviour->rotAim, actorInstance->actorInstance.orientation);
}

/* --------------------------------------------------------------------------------
   Function : RespondToXDoorCollision
   Purpose : Called whent the player touches a X door
   Parameters : collider - character who touched door, XDoor - the x door itself
   Returns : 
   Info : 
*/
int RespondToXDoorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *XDoor)
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;
	float		tempFloat,ang;

	if (!XDoorBook)
	{
		PAGE		*page;

		TEXTBOX		*text;

		XDoorBook = new BOOK();

		XDoorBook->setBookRect(RECTANGLE(-0.45f, 0.0f, 0.0f, 0.45f));

		page = XDoorBook->addPage("galaxy");

		page->insertItem(text = new TEXTBOX(STR_PLANET_X_INTRO));

		text->setWrap(true);
		text->setXAlign(PIFLAG_XALIGN_LEFT);
		text->setYAlign(PIFLAG_YALIGN_TOP);
	}

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,XDoor);
		return FALSE;
	}

	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	// PP: if collider has no character info, we shouldn't go any further
	if((collider->characterInfo == NULL)||(collider->actorStatus == NULL))
	{
		return FALSE;
	}

	// TP: Check if taz is in spin mode
	if (collider->actorStatus->currentState != STATE_MOVE) return FALSE;

	// TP: Check if allowed to enter the level specified
	if ((XDoor->xdoorInfo->level!=-1)&&(!AllowedToEnterLevel(XDoor->xdoorInfo->level))) return FALSE;
	if ((map.sceneNumber==SCENE_ZOOHUB)&&(!AllowedToEnterLevel(SCENE_CITYHUB))) return FALSE;

	// TP: Check door is not doing anything
	if (XDoor->xdoorInfo->state != XDOOR_INACTIVE) return FALSE;

	// TP: Check taz is in front of the door
	bmVectorSub(tempVector,collider->actorInstance.position,XDoor->actorInstance.position);
	tempFloat = Aabs(GetAngleAroundYFromVector(tempVector) + PI);
	ang = GetAngleAroundYFromQuat(XDoor->actorInstance.orientation);
	tempFloat = FindShortestAngleSigned(tempFloat,ang);
	if (bmFAbs(tempFloat) > PI/4) return FALSE;

	// TP: Move or snap Taz into correct position
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-0.89f),1.0f);

	bmQuatToMatrix(tempMatrix,XDoor->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);

	bmVectorAdd(collider->actorInstance.position,XDoor->actorInstance.position,tempVector);

	// TP: face door
	FaceDoor(collider, XDoor);

	// TP: Queue correct animations for Taz and XDoor with collision
	XDoor->xdoorInfo->state = XDOOR_ENTER;
	PlayAnimationByName(XDoor,"xdoorenter",1.0f,0,0,0.0f,NONE);
	FlushAnimationQueue(collider);
	PlayAnimationByName(collider,"xdoorenter",1.0f,0,0,0.0f,DEATH);

	SetShadowNode(collider,collider->actorInstance.rootNodeInstance->children,NULL);

	// TP: Disable player control
	RequestStateChange(collider,STATE_ENTERINGXDOOR);

	SendTrigger(XDoor->xdoorInfo->startEnter, XDoor->actorInstance.position);

	collider->actorStatus->xDoorInUse = XDoor;

	collider->flags |= ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;

	if (XDoor->xdoorInfo->level == -1)
		xDoorEnter = gameStatus.gameTime + 2.4f;

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: FreeXDoor
	Purpose		: Free an x door
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FreeXDoor(ACTORINSTANCE *actorInstance)
{
	if (XDoorBook)
	{
		delete(XDoorBook);

		XDoorBook = 0;
	}

	if (actorInstance->xdoorInfo->startEnter) 
	{
		DeleteTriggerInfo(actorInstance->xdoorInfo->startEnter);
		actorInstance->xdoorInfo->startEnter = NULL;
	}
	if (actorInstance->xdoorInfo->startExit) 
	{
		DeleteTriggerInfo(actorInstance->xdoorInfo->startExit);
		actorInstance->xdoorInfo->startExit = NULL;
	}
	if (actorInstance->xdoorInfo->finishedEnter) 
	{
		DeleteTriggerInfo(actorInstance->xdoorInfo->finishedEnter);
		actorInstance->xdoorInfo->finishedEnter = NULL;
	}
	if (actorInstance->xdoorInfo->finishedExit) 
	{
		DeleteTriggerInfo(actorInstance->xdoorInfo->finishedExit);
		actorInstance->xdoorInfo->finishedExit = NULL;
	}
	if(actorInstance->xdoorInfo->glowInfo)
	{
		SAFE_FREE(actorInstance->xdoorInfo->glowInfo);
		actorInstance->xdoorInfo->glowInfo = NULL;
	}
}

#define XDOORLOCK "extras\\xplanet_door_lock.obe"

/*  --------------------------------------------------------------------------------
	Function	: CreateActorInstanceXDoor
	Purpose		: Creates and initialises a x door
	Parameters	: name of object, instance name, position, orientation, scale, rootNode, node
	Returns		: true if created, fals if not
	Info		: 
*/
bool CreateActorInstanceXDoor(char *name, char *instance, uint32 destDoor, TBVector position, TBVector orientation, TBVector scale, CPMNode *rootNode, CPMNode *node)
{
	TBVector		tempVector;
	float			tempFloat;
	int				tempInt;
	ACTORINSTANCE	*actorInstance;
	char			*tempString;
	char			string[256];

	tempString = node->GetAttrib("model");
	if (tempString)
	{
		sprintf(string,"extras\\%s",tempString);
	}
	else
	{
		strcpy(string,"extras\\xplanet_door.obe");
	}

	if (!FindActorInActorList(string))
	{
		LoadActor(string,map.mapName);
	}

	// TP: Best load in a lock also
	if (!FindActorInActorList(XDOORLOCK))
	{
		LoadActor(XDOORLOCK, map.mapName);
	}

	bmVectorSet(tempVector,1.0f,1.0f,1.0f,1.0f);
	actorInstance = CreateActorInstance(&map.animatedInstances,string,instance,position,tempVector,orientation);

	if (!actorInstance)
	{
		bkPrintf("*** WARNING *** Could not create instance %s for planet X door.\n",instance);
		return false;
	}

	actorInstance->special = SPECIALTYPE_XDOOR;

	actorInstance->xdoorInfo = (XDOORINFO*)ZALLOC(sizeof(XDOORINFO));

	if (!actorInstance->xdoorInfo)
	{
		bkPrintf("*** WARNING *** Could not allocate memory for planet X door %s.\n",instance);
		return false;
	}

	actorInstance->xdoorInfo->destDoorCRC = destDoor;

	actorInstance->xdoorInfo->startEnter = GetTrigger("STARTENTER",node);
	if (!actorInstance->xdoorInfo->startEnter)
	{
		actorInstance->xdoorInfo->startEnter = GetTrigger("STARTENTER",rootNode);
	}
	actorInstance->xdoorInfo->finishedEnter = GetTrigger("FINISHEDENTER",node);
	if (!actorInstance->xdoorInfo->finishedEnter)
	{
		actorInstance->xdoorInfo->finishedEnter = GetTrigger("FINISHEDENTER",rootNode);
	}

	if (node->GetAttrib("DELAY",tempFloat))
	{
		actorInstance->xdoorInfo->delayToNextDoor = tempFloat;
	}
	else
	{
		if (node->GetAttrib("DELAY",tempInt))
		{
			actorInstance->xdoorInfo->delayToNextDoor = (float)tempInt;
		}
		else
		{
			actorInstance->xdoorInfo->delayToNextDoor = 0.0f;
		}
	}

	actorInstance->xdoorInfo->startExit = GetTrigger("STARTEXIT",node);
	if (!actorInstance->xdoorInfo->startExit)
	{
		actorInstance->xdoorInfo->startExit = GetTrigger("STARTEXIT",rootNode);
	}

	actorInstance->xdoorInfo->finishedExit = GetTrigger("FINISHEDEXIT",node);
	if (!actorInstance->xdoorInfo->finishedExit)
	{
		actorInstance->xdoorInfo->finishedExit = GetTrigger("FINISHEDEXIT",rootNode);
	}

	if (tempString = node->GetAttrib("level"))
	{
		actorInstance->xdoorInfo->level = FindLevel(tempString);
	}
	else
	{
		actorInstance->xdoorInfo->level = -1;
	}

	actorInstance->xdoorInfo->state = XDOOR_INACTIVE;

	// TP: if the door is locked then add lock
	if ((actorInstance->xdoorInfo->level!=-1)&&(!AllowedToEnterLevel(actorInstance->xdoorInfo->level)))
	{
		CreateActorInstance(&map.levelInstances, XDOORLOCK, "doorlock", position, tempVector, orientation);
	}

	if ((map.sceneNumber == SCENE_ZOOHUB) && (!AllowedToEnterLevel(SCENE_CITYHUB)))
	{
		CreateActorInstance(&map.levelInstances, XDOORLOCK, "doorlock", position, tempVector, orientation);
	}

	SetCollisionCallback(actorInstance,RespondToXDoorCollision);
	AddActorToCollisionCache(&collisionCache,actorInstance,CACHEFLAGS_LIVEUPDATE);

	actorInstance->xdoorInfo->glowInfo = CreateGlowAroundInstance(actorInstance, METERTOUNIT(1.5f));
	ASSERTM(actorInstance->xdoorInfo->glowInfo, "No Memory available for x-door glows");

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: EnterSceneUsingXDoor
	Purpose		: Character has entered the scene using a x door
	Parameters	: actorInstance
	Returns		: true if door found, false if not
	Info		: 
*/
bool EnterSceneUsingXDoor(ACTORINSTANCE *actorInstance, uint32 xDoorCRC)
{
	ACTORINSTANCE	*xDoor;
	TBVector		tempVector;
	TBMatrix		tempMatrix;
	float			tempFloat;

	xDoor = FindActorInstanceInMap(NULL,xDoorCRC,&map);

	if (!xDoor) 
	{
		bkPrintf("*** WARNING *** Could not find door to enter level.\n");
		return false;
	}

	if (xDoor->special != SPECIALTYPE_XDOOR) return false;

	actorInstance->actorStatus->xDoorInUse = xDoor;

	// TP: Move or snap Taz into correct position
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-0.89f),1.0f);
	bmQuatToMatrix(tempMatrix,xDoor->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(actorInstance->actorInstance.position,xDoor->actorInstance.position,tempVector);

	// TP: face door
	FaceDoor(actorInstance,xDoor);
/*	bmVectorSub(tempVector,xDoor->actorInstance.position,actorInstance->actorInstance.position);
	tempFloat = GetAngleAroundYFromVector(tempVector);
	CharacterRotateToAngle(actorInstance,0,-tempFloat,0.0f,0.0f);*/

	SetShadowNode(actorInstance,actorInstance->actorInstance.rootNodeInstance->children,NULL);

	// TP: Queue correct animations for Taz and XDoor with collision
	xDoor->xdoorInfo->state = XDOOR_EXIT;
	PlayAnimationByName(xDoor,"xdoorexit",1.0f,0,0,0.0f,NONE);
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"xdoorexit",1.0f,0,0,0.0f,DEATH);

	// TP: Send exit messages
	SendTrigger(xDoor->xdoorInfo->startExit, xDoor->actorInstance.position);

	actorInstance->flags |= ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: CheckForXDoorSkip
	Purpose		: Check if the cameracase can be skipped
	Parameters	: xDoor
	Returns		: 
	Info		: 
*/

void CheckForXDoorSkip(ACTORINSTANCE *xDoor)
{
	char			*finishCam, *platform, *mainCamera;
	CAMERACASE		*camcase;
	ACTORINSTANCE	*actorInstance;

	finishCam = "finish cam";
	platform = "door exit camstwirl path02";
	mainCamera = "door exit camstwirl";

	// TP: if playing correct camera case
	// TP: stop existing camera case
	// TP: start finish cam
	// TP: stop platform
	// TP: zero delay
	if (OKBUTTON)
	{
		FORCE_OKBUTTON_DEBOUNCE;
		if ( (camera[0].cameraCase)&&(camera[0].cameraCase->caseName==bkCRC32((uchar*)mainCamera,strlen(mainCamera),0)) )
		{
			if (camcase = FindCameraCase(finishCam))
			{
				StopCameraCase(&camera[0], camera[0].cameraCase,true);
				StartCameraCase(camcase, &camera[0]);
				if (actorInstance = FindActorInstanceInMap(platform,0,&map))
				{
					if (actorInstance->pathInfo)
					{
						ChangePathState(actorInstance->pathInfo,PATHSTATE_WAITING);
						SetPathToStart(actorInstance->pathInfo);
					}
				}
				else
				{
					bkPrintf("***WARNING*** Could not find xplanet platform: door exit camstwirl path02\n");
				}
				XDoorBook->close();
				xDoor->xdoorInfo->delay = 0;
			}
			else
			{
				bkPrintf("***WARNING*** Could not find xplanet cameracase: finish cam\n");
			}
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: UpdateXDoorState
	Purpose		: Update called during the x door move
	Parameters	: actorInstance of character using door
	Returns		: true if move finished
	Info		: 
*/
bool UpdateXDoorState(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE	*xDoor;
	ACTORINSTANCE	*dest;
	TBVector		tempVector;
	TBMatrix		tempMatrix;
	float			tempFloat;

	// TP: Get hold of door being used
	xDoor = actorInstance->actorStatus->xDoorInUse;

	switch(xDoor->xdoorInfo->state)
	{
	case XDOOR_ENTER:
		if ((xDoorEnter > 0.0f && gameStatus.gameTime > xDoorEnter)&&(xDoor->xdoorInfo->level == -1))
		{
			camera[0].fade.StartFadeDown(FADETYPE_SWIPE, 0.8f);
		
			camera[0].fade.SetSwipeDirection(RAD(90), RAD(-90));
			camera[0].fade.FadeUpOnCompletion(true);

			xDoorEnter = -1;
		}

		if (!CheckIfPlayingAnimation(actorInstance,"xdoorenter"))
		{
			// TP: taz has finished entering the door

			// TP: send any enter messages
			SendTrigger(xDoor->xdoorInfo->finishedEnter, xDoor->actorInstance.position);

			// TP: if no level change then switch to dest door
			if (xDoor->xdoorInfo->level == -1)
			{
				if (xDoor->xdoorInfo->destDoorCRC)
				{
					// TP: find new door and switch
					dest = FindActorInstanceInMap(NULL,xDoor->xdoorInfo->destDoorCRC,&map);
					if ((dest)&&(dest->special==SPECIALTYPE_XDOOR))
					{
						// TP: reset current door
						xDoor->xdoorInfo->state = XDOOR_INACTIVE;
						FlushAnimationQueue(xDoor);
						PlayAnimationByName(xDoor,"xdoorexit",1.0f,0,0,0.0f,NONE);

						// TP: move taz to new position
						SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-0.89f),1.0f);
						bmQuatToMatrix(tempMatrix,dest->actorInstance.orientation);
						bmMatMultiplyVector(tempMatrix,tempVector);
						bmVectorAdd(actorInstance->actorInstance.position,dest->actorInstance.position,tempVector);
						
						// TP: rotate to face new door
						FaceDoor(actorInstance,dest);
/*
						bmVectorSub(tempVector,dest->actorInstance.position,actorInstance->actorInstance.position);
						tempFloat = GetAngleAroundYFromVector(tempVector);
						CharacterRotateToAngle(actorInstance,0,-tempFloat,0.0f,0.0f);*/

						// TP: start new door
						if (xDoor->xdoorInfo->delayToNextDoor)
						{
							dest->xdoorInfo->delay = xDoor->xdoorInfo->delayToNextDoor;
							dest->xdoorInfo->state = XDOOR_LIMBO;
							actorInstance->flags |= ACTORFLAG_DONTDRAW;
						}
						else
						{
							dest->xdoorInfo->state = XDOOR_EXIT;
							FlushAnimationQueue(dest);
							PlayAnimationByName(dest,"xdoorexit",1.0f,0,0,0.0f,NONE);
							FlushAnimationQueue(actorInstance);
							PlayAnimationByName(actorInstance,"xdoorexit",1.0f,0,0,0.0f,DEATH);
							SendTrigger(dest->xdoorInfo->startExit, dest->actorInstance.position);
						}

						// TP: switch the door watching instance
						actorInstance->actorStatus->xDoorInUse = dest;
					}
					else
					{
						// TP: no dest door or level change so exit current door
						bkPrintf("*** WARNING *** Could not find destination instance for xdoor or destination is not a door%s.\n",xDoor->instanceName);

						// TP: exit current door
						xDoor->xdoorInfo->state = XDOOR_EXIT;
						FlushAnimationQueue(xDoor);
						PlayAnimationByName(xDoor,"xdoorexit",1.0f,0,0,0.0f,NONE);
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,"xdoorexit",1.0f,0,0,0.0f,DEATH);
					}
				}
				else
				{
					// TP: no dest door or level change so exit current door
					bkPrintf("*** WARNING *** No destination door or level set for xdoor %s.\n",xDoor->instanceName);

					// TP: exit current door
					xDoor->xdoorInfo->state = XDOOR_EXIT;
					FlushAnimationQueue(xDoor);
					PlayAnimationByName(xDoor,"xdoorexit",1.0f,0,0,0.0f,NONE);
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance,"xdoorexit",1.0f,0,0,0.0f,DEATH);
				}
			}
			else
			{
				// TP: change level
				camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,xDoor->xdoorInfo->level,0.5,xDoor->xdoorInfo->destDoorCRC);
				SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
			}
		}
		break;
	case XDOOR_LIMBO:
// PP: REMOUT: Now handled by the Book List			XDoorBook->update();			

		if (camera[0].fade.GetFadeState() == FADESTATE_OFF && xDoorEnter == -1 && xDoor->xdoorInfo->delay > 2.0f)
		{
			xDoorEnter = gameStatus.gameTime + 8.1f;
			XDoorBook->open();
		}
		else if (camera[0].fade.GetFadeState() == FADESTATE_OFF && xDoorEnter && xDoorEnter < gameStatus.gameTime)
		{
			XDoorBook->close();

			xDoorEnter = 0;
		}

#ifdef TONY
		CheckForXDoorSkip(xDoor);
#endif

		if ((xDoor->xdoorInfo->delay-=fTime)<=0.0f)
		{
			xDoor->xdoorInfo->state = XDOOR_EXIT;
			FlushAnimationQueue(xDoor);
			PlayAnimationByName(xDoor,"xdoorexit",1.0f,0,0,0.0f,NONE);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"xdoorexit",1.0f,0,0,0.0f,DEATH);

			SendTrigger(xDoor->xdoorInfo->startExit, xDoor->actorInstance.position);

			actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
		}
		break;
	case XDOOR_EXIT:
		if (!CheckIfPlayingAnimation(actorInstance,"xdoorexit"))
		{
			// TP: taz has finished exiting the door, rotate 180
			FaceDoor(actorInstance, xDoor, 0.0f);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.0f,NONE);
			AnimateActorInstance(actorInstance);
			baPrepActorInstance(&actorInstance->actorInstance,true);

			// TP: Send exit messages
			SendTrigger(xDoor->xdoorInfo->finishedExit, xDoor->actorInstance.position);

			xDoor->xdoorInfo->state = XDOOR_INACTIVE;

			SetShadowNode(actorInstance,NULL,NULL);

			actorInstance->flags &= ~(ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);
			
			// TP: all done 
			return true;
		}
		break;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateXDoorSparkles
	Purpose		: 
	Parameters	: actorInstance of door
	Returns		: 
	Info		: 
*/

void UpdateXDoorSparkles(ACTORINSTANCE *actorInstance)
{
	// CMD: if door is locked don't create any sparkly stuff
	switch(map.sceneNumber)
	{
	case SCENE_ZOOHUB:
		if(AllowedToEnterLevel(SCENE_CITYHUB)&&AllowedToEnterLevel(actorInstance->xdoorInfo->level))
		{
			if(actorInstance->xdoorInfo->glowInfo)
			{
				UpdatePlinthGlowAroundActorInstance(actorInstance->xdoorInfo->glowInfo);
			}
		}
		break;
	default:
		if(AllowedToEnterLevel(actorInstance->xdoorInfo->level))
		{
			if(actorInstance->xdoorInfo->glowInfo)
			{
				UpdatePlinthGlowAroundActorInstance(actorInstance->xdoorInfo->glowInfo);
			}
		}
	}
}