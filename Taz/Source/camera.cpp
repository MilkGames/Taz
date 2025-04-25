// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : camera.cpp
//   Purpose : camera functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "collisioncache.h"
#include "camera.h"
#include "control.h"
#include "maths.h"
#include "main.h"
#include "scene.h"
#include "quakecam.h"
#include "cameracase.h"
#include "trail.h"
#include "characters.h"
#include "status.h"
#include "player.h"
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "genericai.h"
#include "sfx.h"					// PP: sound effect rountines
#include "util.h"					// PP: general utility functions
#include "smashcamera.h"
#include "costumes.h"
#include "Cheats.h"					// PP: Cheats
#include "display.h"				// PP: provide basic display setup and managment functions
#include "prompt.h"

CAMERAINFO	camera[MAX_SCREENS];	// TP: One camera per screen

// PP: copy of the last camera sent.
// PP: If you want to set up new viewpoints during the draw, re-send this when you're finished.
// PP: By that time the original camera may have been updated (I think?).
static CAMERAINFO	lastSentCameraCopy;// PP: no real need to be static I suppose, jus' keepin' it safe.

float CAMERADIST = 500.0f;
float CAMERAANGLE = 0.3f;


/*	--------------------------------------------------------------------------------
	Function 	: SimplifyCamera
	Purpose 	: take all the fancy stuff off a camera, so you can control it simply by its pos & lookat
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: These bloomin' fancy cameras!  They've got mind of their own!
*/
void SimplifyCamera(CAMERAINFO* const cameraInfo)
{
	DisableCameraLookAtFollowActorInstance(cameraInfo);

	// PP: initialise offsets...

	SETVECTOR(cameraInfo->posOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->lookAtOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	cameraInfo->tiltOffset=0.0f;

	// PP: initialise rumble data...

	SETVECTOR(cameraInfo->rumbleData.posOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.lookAtOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.angOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.angOffset, 0.0f, 0.0f, 0.0f, 0.0f);

	// PP: don't want camera collision
	SetCameraCollision(cameraInfo, false);

	// PP: don't want to control the camera with the pad
	cameraInfo->flags &= ~CAMERA_PADCONTROL;

	cameraInfo->flags &= ~CAMERA_SMARTCAM;

	// PP: don't want lerping
	SetCameraSmooth(cameraInfo, 1.0f, 1.0f);

	// PP: no fixed distance to lookat
	SetFixedDistanceToLookAt(cameraInfo, 0.0f);

	// PP: no fixed elevation above target
	DisableCameraAngleAboveHorizontal(cameraInfo);
}


/* --------------------------------------------------------------------------------
   Function : InitCamera
   Purpose : Init camera structure
   Parameters : CAMERAINFO, origin, lookAt
   Returns : 
   Info : 
*/
void InitCamera(CAMERAINFO *cameraInfo, TBVector origin, TBVector lookAt,bool initStructure)
{
	if (initStructure) InitActorStructures(&cameraInfo->collisionActor);

	bmVectorCopy(cameraInfo->pos,origin);
	bmVectorCopy(cameraInfo->lookAt,lookAt);

	bmVectorCopy(cameraInfo->destinationPos,origin);
	bmVectorCopy(cameraInfo->destinationLookAt,lookAt);
	cameraInfo->actorInstanceLookAt = NULL;
	cameraInfo->ourActorInstanceLookAt = NULL;						// PP: must initialise ACTORINSTANCE look-at ptr to NULL to prevent a crash further on
	cameraInfo->actorInstanceNodeLookAt = NULL;
	cameraInfo->actorInstancePos = NULL;
	cameraInfo->actorInstanceNodePos = NULL;
	cameraInfo->fixedDistanceToLookAt = 0.0f;
	cameraInfo->angleAboveHorizontal = 0.0f;
	cameraInfo->posSmooth = 1.0f;
	cameraInfo->lookAtSmooth = 1.0f;
	cameraInfo->flags = CAMERA_DEFAULT_FLAGS;						// PP: set default flags for the camera
	cameraInfo->tilt = RAD(0.0f);
	cameraInfo->cameraCase = NULL;
	cameraInfo->ourActorInstanceLookAt = NULL;
	cameraInfo->defaultAngleAboveHorizontal = CAMERAANGLE;
	cameraInfo->defaultFixedDistance = CAMERADIST;
	cameraInfo->crcQueuedCase = 0;
	cameraInfo->queueDelay = 0.0f;
	cameraInfo->smashInfo = NULL;
	// PP: initialise offsets...

	SETVECTOR(cameraInfo->posOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->lookAtOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	cameraInfo->tiltOffset=0.0f;

	// PP: initialise rumble data...

	SETVECTOR(cameraInfo->rumbleData.posOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.lookAtOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.angOffsetVelocity, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->rumbleData.angOffset, 0.0f, 0.0f, 0.0f, 0.0f);
}

/* --------------------------------------------------------------------------------
   Function : CreateCameraCollSphere
   Purpose : Create a collision sphere for the camera
   Parameters : CAMERAINFO, sphere offset, radius, maxSpeed(not currently supported)
   Returns : 
   Info : 
*/
void CreateCameraCollSphere(CAMERAINFO *cameraInfo, TBVector offset, float radius, float maxSpeed)
{
	// create collision actor for camera
	if (cameraInfo->collisionActor.actorBehaviour==NULL)
		CreateActorInstanceBehaviour(&cameraInfo->collisionActor);
	if(cameraInfo->collisionActor.actorStatus == NULL)
		CreateActorInstanceStatus(&cameraInfo->collisionActor);
	SetActorInertia(cameraInfo->collisionActor.actorBehaviour,0.0f);
	SetActorMinFricAngle(cameraInfo->collisionActor.actorBehaviour,1.0f);
	SetActorMaxSpeed(cameraInfo->collisionActor.actorBehaviour,maxSpeed);
	SetActorCollSphere(cameraInfo->collisionActor.actorBehaviour,offset,radius);
	cameraInfo->collisionActor.actorBehaviour->physics.flags|=PHYSFLAG_NOGRAVITY;
}

/* --------------------------------------------------------------------------------
   Function : DestroyCamera
   Purpose : destroy camera structure
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DestroyCamera(CAMERAINFO *cameraInfo)
{
	if (cameraInfo->cameraCase) StopCameraCase(cameraInfo,cameraInfo->cameraCase, false);
	if (cameraInfo->collisionActor.actorBehaviour) FREE(cameraInfo->collisionActor.actorBehaviour);
	if (cameraInfo->collisionActor.actorStatus) FREE(cameraInfo->collisionActor.actorStatus);
	if(cameraInfo->smashInfo) FreeScreenSmashEffect(cameraInfo);
	cameraInfo->fade.SetFadeTint(0,0,0,0);
}

/*  --------------------------------------------------------------------------------
	Function	: SmartCam
	Purpose		: Handle awkward moments when the camera gets stuck
	Parameters	: cameraInfo
	Returns		: 
	Info		: 
*/

float MAX_RESISTANCE = -0.8f;

void SmartCam(CAMERAINFO *cameraInfo)
{
	float len;
	TBVector	tempVector;
	float dot;

	if (cameraInfo->ourActorInstanceLookAt)
	{
		if (cameraInfo->ourActorInstanceLookAt->playerNo == -1)
		{
			// TP: if the camera is not looking at Taz then we can do the smart cam
			return;
		}
	}
	else
	{
		return;
	}

	if (cameraInfo->flags & CAMERA_CATCHUP)
	{
		// TP: camera is trying to return to taz
		SetCameraCollision(cameraInfo,false);

		if (cameraInfo->followBehind.currentIndex==-1)
		{
			TBVector pDest;

			cameraInfo->followBehind.currentIndex = trailFindNearestPoint(cameraInfo->ourActorInstanceLookAt, cameraInfo->pos, cameraInfo->followBehind.curDest);

			if (cameraInfo->followBehind.currentIndex!=-1)
			{
				bmVectorCopy(cameraInfo->destinationPos, cameraInfo->followBehind.curDest);
			}
		}
		else
		{
			// TP: test for distance to current Index making sure it is still valid
			if (ReachedPoint(cameraInfo->pos, cameraInfo->followBehind.curDest, METERTOUNIT(0.5f)))
			{
				trailAdvanceToNext(cameraInfo->ourActorInstanceLookAt, &cameraInfo->followBehind.currentIndex);
				trailGetIndexedPoint(cameraInfo->ourActorInstanceLookAt, cameraInfo->followBehind.currentIndex, cameraInfo->followBehind.curDest);
			}
		}

		if (cameraInfo->followBehind.currentIndex!=-1)
		{
			bmVectorCopy(cameraInfo->destinationPos, cameraInfo->followBehind.curDest);
		}

		SetCameraSmooth(cameraInfo, 0.2f, 0.2f);

		// TP: test to see if camera is ok again
		bmVectorSub(tempVector, cameraInfo->destinationLookAt, cameraInfo->pos);
		len = bmVectorLen(tempVector);

		// TP: if less than a meter behind or there is no camera collision then return to normal
		if((len < (METERTOUNIT(1.0f) + cameraInfo->fixedDistanceToLookAt))||(!cameraInfo->collisionActor.actorBehaviour->physics.coll)) 
		{
			bkPrintf("Camera catchup disabled!\n");
			cameraInfo->flags &= ~CAMERA_CATCHUP;
			SetCameraSmooth(cameraInfo, defaultPosSmooth, defaultLookatSmooth);
		}
		else
		{
			// TP: check for ok collision
			if (cameraInfo->collisionActor.actorBehaviour->physics.coll)
			{
				// TP: dot product between the direction the camera is trying to move and the average normal from the last collision
				bmVectorNorm(tempVector, tempVector);
				tempVector[W] = 1.0f;
				dot = bmVectorDot(tempVector, cameraInfo->collisionActor.actorBehaviour->physics.lastAverageNormal);

				// TP: if the camera is colliding against an object facing towards it then we DO have a problem
				if(dot >= MAX_RESISTANCE)
				{
					bkPrintf("Camera catchup disabled!\n");
					cameraInfo->flags &= ~CAMERA_CATCHUP;
					SetCameraSmooth(cameraInfo, defaultPosSmooth, defaultLookatSmooth);
				}
			}
		}
	}
	else
	{
		// TP: camera is fine
		SetCameraCollision(cameraInfo,true);
		cameraInfo->followBehind.currentIndex=-1;

		// TP: check to see if the camera has a problem
		if(cameraInfo->collisionActor.actorBehaviour)
		{
			bmVectorSub(tempVector, cameraInfo->destinationLookAt, cameraInfo->pos);
			len = bmVectorLen(tempVector);

			// TP: if the camera is lagging behind by more than a meter over the requested distance and there is a collision then we may have a problem
			if((len > (METERTOUNIT(1.0f) + cameraInfo->fixedDistanceToLookAt)) && (cameraInfo->collisionActor.actorBehaviour->physics.coll)) 
			{
				// TP: dot product between the direction the camera is trying to move and the average normal from the last collision
				bmVectorNorm(tempVector, tempVector);
				tempVector[W] = 1.0f;
				dot = bmVectorDot(tempVector, cameraInfo->collisionActor.actorBehaviour->physics.lastAverageNormal);

				// TP: if the camera is colliding against an object facing towards it then we DO have a problem
				if(dot < MAX_RESISTANCE)
				{
					if(cameraInfo->ourActorInstanceLookAt)
					{
						bkPrintf("Camera catchup enabled!\n");
						EnableCameraCatchup(cameraInfo, cameraInfo->ourActorInstanceLookAt);
					}
				}
			}
		}
	}
}
	
/* --------------------------------------------------------------------------------
   Function : UpdateCamera
   Purpose : Update camera info, move camera etc.
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void UpdateCamera(CAMERAINFO *cameraInfo)
{
	TBVector temp,temp2;
	float	dist,height;
	float	posSmooth,destSmooth;
	TBVector	newPos;
	float	actorAngle,cameraAngle,angleDiff;
	float	angleAboveHorizontal = cameraInfo->angleAboveHorizontal;

	// TP: Update any queued camera cases
	if (cameraInfo->crcQueuedCase)
	{
		cameraInfo->queueDelay -= fTime;
		if (cameraInfo->queueDelay <= 0.0f)
		{
			CAMERACASE *camCase = FindCameraCase(cameraInfo->crcQueuedCase);
			if (camCase)
			{
				StartCameraCase(camCase,&camera[0]);
			}
			cameraInfo->crcQueuedCase = 0;
			cameraInfo->queueDelay = 0.0f;
		}
	}

	// update override camera
	if (cameraInfo->cameraCase)
	{
		UpdateCameraCase(cameraInfo,cameraInfo->cameraCase);
	}
	else
	{
		// PP: quick & easy - override the camera completely if the player is looking at the watch...

		if(gameStatus.multiplayer.numScreens == 1)// PP: don't want any complications yet - stick to camera[0] only
		{	
			if(CheatActive(CHEAT_DIGITALWATCH))
			{
				if(watchLook)
				{
					LookAtWatch();
				}
			}
		}
	}

	if (camera->flags&CAMERA_CUTSCENE)
	{
		// TP: if cutscene then make sure the new position of the camera dummys are read before the camera update
		if (cameraInfo->actorInstanceLookAt)
		{
			if (cameraInfo->actorInstanceNodeLookAt)
			{
				baGetNodesWorldPosition(cameraInfo->actorInstanceLookAt, cameraInfo->actorInstanceNodeLookAt, cameraInfo->destinationLookAt);
			}
			else
			{
				bmVectorCopy(cameraInfo->destinationLookAt,cameraInfo->actorInstanceLookAt->position);
			}
			bmVectorAdd(cameraInfo->destinationLookAt,cameraInfo->destinationLookAt,cameraInfo->actorInstanceLookAtOffset);
		}
		if (cameraInfo->actorInstancePos)
		{
			if (cameraInfo->actorInstanceNodePos)
			{
				baGetNodesWorldPosition(cameraInfo->actorInstancePos, cameraInfo->actorInstanceNodePos, cameraInfo->destinationPos);
			}
			else
			{
				bmVectorCopy(cameraInfo->destinationPos,cameraInfo->actorInstancePos->position);
			}
		}
	}

	// smooth and update camera movement
	posSmooth = cameraInfo->posSmooth*speedRatio;
	if (posSmooth>1.0f) posSmooth = 1.0f;
	destSmooth = cameraInfo->lookAtSmooth*speedRatio;
	if (destSmooth>1.0f) destSmooth = 1.0f;

	cameraInfo->readFlags &= ~CAMERA_RFREACHEDPOS;

	// move actual camera positions
	if (cameraInfo->posSmooth!=1.0f)
	{
		bmVectorLerp(newPos,cameraInfo->pos,cameraInfo->destinationPos,posSmooth);

		if(bmVectorDistance(cameraInfo->pos,cameraInfo->destinationPos) < METERTOUNIT(0.1f))
			cameraInfo->readFlags |= CAMERA_RFREACHEDPOS;
	}
	else
	{
		bmVectorCopy(newPos,cameraInfo->destinationPos);
		cameraInfo->readFlags |= CAMERA_RFREACHEDPOS;
	}

	if (cameraInfo->collisionActor.actorBehaviour)
	{
		// do collision for camera

		// setup position based on old position
		// setup velocity
		bmVectorCopy(cameraInfo->collisionActor.actorInstance.position,cameraInfo->pos);
		bmVectorSub(temp,newPos,cameraInfo->pos);
		bmVectorCMul(temp,temp,fps);

		if (bmVectorLen(temp) > worldMaxGravSpeed)
		{
			bmVectorCopy(cameraInfo->pos,newPos);
		}
		else
		{
			SetActorVelocity(cameraInfo->collisionActor.actorBehaviour,temp);
			ProcessCollisionForActor(&collisionCache, &cameraInfo->collisionActor, PROCESSCOLLFLAG_DONTAFFECTOBJECTS);

			if (!(cameraInfo->flags & CAMERA_NOCOLLISION)) 
			{
				bmVectorCopy(cameraInfo->pos,cameraInfo->collisionActor.actorInstance.position);
				bmVectorSub(temp,cameraInfo->lookAt,cameraInfo->pos);
				bmVectorNorm(temp,temp);
			}
			else
			{
				bmVectorCopy(cameraInfo->pos,newPos);
			}
		}
	}
	else
	{
		bmVectorCopy(cameraInfo->pos,newPos);
	}

	if (cameraInfo->posSmooth!=1.0f)
	{
		bmVectorLerp(cameraInfo->lookAt,cameraInfo->lookAt,cameraInfo->destinationLookAt,destSmooth);
	}
	else
	{
		bmVectorCopy(cameraInfo->lookAt,cameraInfo->destinationLookAt);
	}

	// update destination positions for next frame
	if (~camera->flags&CAMERA_CUTSCENE)
	{
		if (cameraInfo->actorInstanceLookAt)
		{
			if (cameraInfo->actorInstanceNodeLookAt)
			{
				baGetNodesWorldPosition(cameraInfo->actorInstanceLookAt, cameraInfo->actorInstanceNodeLookAt, cameraInfo->destinationLookAt);
			}
			else
			{
				bmVectorCopy(cameraInfo->destinationLookAt,cameraInfo->actorInstanceLookAt->position);
			}
			bmVectorAdd(cameraInfo->destinationLookAt,cameraInfo->destinationLookAt,cameraInfo->actorInstanceLookAtOffset);
		}
		if (cameraInfo->actorInstancePos)
		{
			if (cameraInfo->actorInstanceNodePos)
			{
				baGetNodesWorldPosition(cameraInfo->actorInstancePos, cameraInfo->actorInstanceNodePos, cameraInfo->destinationPos);
			}
			else
			{
				bmVectorCopy(cameraInfo->destinationPos,cameraInfo->actorInstancePos->position);
			}
		}
	}
	if ((cameraInfo->flags&CAMERA_ROTATEBEHIND)&&(cameraInfo->actorInstanceLookAt))
	{
		// rotate the camera around behind the actor

		// get actors angle
		actorAngle = GetAngleAroundYFromQuat(cameraInfo->actorInstanceLookAt->orientation);
		// get cameras angle
		bmVectorSub(temp, cameraInfo->destinationPos,cameraInfo->destinationLookAt);
		cameraAngle = bmATan2(temp[X],temp[Z]);
		cameraAngle = Aabs(-(cameraAngle));

		angleDiff = FindShortestAngleSigned(actorAngle,cameraAngle);
		if (bmFAbs(angleDiff) > (cameraInfo->rotRadsPerSecond/fps))
		{
			if (angleDiff > 0.0f)
			{
				angleDiff = cameraInfo->rotRadsPerSecond/fps;
			}
			else
			{
				angleDiff = -cameraInfo->rotRadsPerSecond/fps;
			}
		}

		// rotate destinationPos around desinationLookAt by angleDiff
		bmVectorRotateY(temp,temp,-angleDiff);
		bmVectorAdd(cameraInfo->destinationPos,cameraInfo->destinationLookAt,temp);
	}

	if((cameraInfo->flags&CAMERA_CHASECAM)&&(cameraInfo->actorInstanceLookAt))
	{
		//get actors velocity's angle
		actorAngle = GetAngleAroundYFromVector(cameraInfo->chaseCamVel) + PI;
		//get cameras angle
		bmVectorSub(temp, cameraInfo->destinationPos, cameraInfo->destinationLookAt);
		cameraAngle = bmATan2(temp[X], temp[Z]);
		cameraAngle = Aabs(-(cameraAngle));

		angleDiff = FindShortestAngleSigned(actorAngle,cameraAngle);
		if (bmFAbs(angleDiff) > (cameraInfo->rotRadsPerSecond/fps))
		{
			if (angleDiff > 0.0f)
			{
				angleDiff = cameraInfo->rotRadsPerSecond/fps;
			}
			else
			{
				angleDiff = -cameraInfo->rotRadsPerSecond/fps;
			}
		}

		// rotate destinationPos around desinationLookAt by angleDiff
		bmVectorRotateY(temp,temp,-angleDiff);
		bmVectorAdd(cameraInfo->destinationPos,cameraInfo->destinationLookAt,temp);
	}
	if (cameraInfo->flags&CAMERA_ANGLEOVERRIDEY)
	{
		// correct y value of destinationPos if angle override
		bmVectorSub(temp,cameraInfo->destinationPos,cameraInfo->destinationLookAt);
		temp[Y] = 0.0f;
		dist = bmVectorLen(temp);
		height = bmTan(angleAboveHorizontal) * dist;
		cameraInfo->destinationPos[Y] = cameraInfo->destinationLookAt[Y] + height;
	}

	if (cameraInfo->fixedDistanceToLookAt)
	{

		bmVectorSub(temp,cameraInfo->destinationPos,cameraInfo->destinationLookAt);

		if (bmVectorLen(temp) == 0.0f)
		{
			// check for destinationLookAt and desinationPos at the same point
			cameraInfo->destinationPos[Z] -= 10.0f;
			bmVectorSub(temp,cameraInfo->destinationPos,cameraInfo->destinationLookAt);
		}

		bmVectorScaleToLength(temp,temp,cameraInfo->fixedDistanceToLookAt);
		bmVectorAdd(cameraInfo->destinationPos,cameraInfo->destinationLookAt,temp);
	}

	if((gameStatus.chrisCam)&&(cameraInfo->flags&CAMERA_SMARTCAM))
	{
		SmartCam(cameraInfo);
	}

	UpdateQuakeCam(cameraInfo);

	UpdateRumbleCam(cameraInfo);		// PP: update 'rumble' effects - a bit like the quake effects only smoother

	UpdateScreenSmashEffect(cameraInfo->smashInfo);		// CMD: updates the smash texture on the screen
}

/* --------------------------------------------------------------------------------
   Function : SendCamera
   Purpose : Send the camera information to babel
   Parameters : (opt/NULL=copy of last camera sent) CAMERAINFO
   Returns : 
   Info : 
*/

extern float stereoDivergence;
extern float stereoSeparation;

void SendCamera(CAMERAINFO *cameraInfo)
{
	VEC				cameraPos;						//temp values for position
	VEC				cameraLookAt;					//look at, and
	float			cameraTilt;						//tilt
	CAMERAINFO*		cam;							// PP: camera whose details we want to send
	extern bool		g_overrideStereoscopy;			// PP: flag specifying that stereoscopy is to be overridden (display,cpp)
	TBQuaternion	camQuat;						// PP: quaternion representing the angle of the camera
	TBQuaternion	offsetQuat;						
	TBQuaternion	tempQuat;
	

	// PP: TOFIX: PROBLEM!!!!!: what about camera cases?

	if(cameraInfo == NULL)
	{
		cameraInfo= &lastSentCameraCopy;
	}

	// PP: add tilt offset to camera tilt
	cameraTilt = cameraInfo->tilt + cameraInfo->tiltOffset;

	// PP: get a ptr to the camera whose other details we want to send...

	if((cameraInfo->cameraCase)&&(!(cameraInfo->flags&CAMERA_LOOKAROUND)))
	{
		cam=cameraInfo->cameraCase->camera;
	}
	else
	{
		cam=cameraInfo;
	}

	// PP: TEST: copy the camera being sent, so's during the draw loop we can switch away from/ back to it without nasty lagging
	lastSentCameraCopy=(*cam);

	// PP: add position offset to camera position
	bmVectorAdd(cameraPos, cam->pos, cam->posOffset);

	// PP: add lookat offset to camera lookat
	bmVectorAdd(cameraLookAt, cam->lookAt, cam->lookAtOffset);

	// PP: =====================

	// PP: determine the camera angle from position, lookat and tilt
	VEC		camDir=cameraLookAt-cameraPos;

	bmDirectionToQuat(tempQuat, -camDir, cameraTilt);// PP: dunno why I have to negate the direction, but I do

	// PP: apply angle offset to camera angle
	bmEulerToQuatZXY(offsetQuat, cam->rumbleData.angOffset[X], cam->rumbleData.angOffset[Y], cam->rumbleData.angOffset[Z]);
	bmQuatMultiply(camQuat, offsetQuat, tempQuat);

	// PP: apply stereoscopic offset
	if(VFX_effectActive(VFX_STEREO))
	{
		if(!g_overrideStereoscopy)
		{
			VFX_adjustCameraForStereoscopy(cameraPos, camQuat);
		}
	}

	// PP: store viewpoint, once offset for stereoscopy
	bmVectorCopy(gameStatus.viewPoint,cameraPos);

	// PP: send the camera position
	bdSetViewPosition(cameraPos[X], cameraPos[Y], cameraPos[Z]);

	// PP: store final camera angle
	bmQuatToEulerYXZ(camQuat, &cameraInfo->cameraState.cameraAngles[X], &cameraInfo->cameraState.cameraAngles[Y], &cameraInfo->cameraState.cameraAngles[Z]);
	bmVectorCopy(gameStatus.viewAng, cameraInfo->cameraState.cameraAngles);

	// PP: send the camera angle...
	
	/*// PP: LEAVE IN!
	// PP: why doesn't this work??!! what's going on
	bdSetViewOrientationFromQuaternion(camQuat);
	/*/

	// PP: don't have a clue why I have to do this...

	TBQuaternion	invQuat;
	
	bmQuatInverse(invQuat, camQuat);
	bdSetViewOrientationFromQuaternion(invQuat);// PP: this works if dir negated

	//*/

	// PP: =====================

	bmVectorCopy(cameraInfo->cameraState.cameraPos, cameraPos);
	bmVectorCopy(cameraInfo->cameraState.cameraLookat, cameraLookAt);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraPosition
   Purpose : Sets the current camera position and camera destination
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraPosition(CAMERAINFO *cameraInfo, TBVector pos)
{
	bmVectorCopy(cameraInfo->pos,pos);
	bmVectorCopy(cameraInfo->destinationPos,pos);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraPositionOffset
   Purpose : Sets the current position offset
   Parameters : CAMERAINFO, (opt/NULL=no offset)TBVector position
   Returns : 
   Info : 
*/

void SetCameraPositionOffset(CAMERAINFO *cameraInfo, TBVector offset)
{
	TBVector	zeroVec;

	if(offset == NULL)
	{
		SETVECTOR(zeroVec, 0.0f, 0.0f, 0.0f, 0.0f);
		offset=zeroVec;
	}

	bmVectorCopy(cameraInfo->posOffset, offset);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtPosition
   Purpose : Sets the current look at position and look at destination
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/

void SetCameraLookAtPosition(CAMERAINFO *cameraInfo, TBVector pos)
{
	bmVectorCopy(cameraInfo->lookAt,pos);
	bmVectorCopy(cameraInfo->destinationLookAt,pos);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtOffset
   Purpose : Sets the current look at position offset
   Parameters : CAMERAINFO, (opt/NULL=no offset)TBVector position
   Returns : 
   Info : 
*/

void SetCameraLookAtOffset(CAMERAINFO *cameraInfo, TBVector offset)
{
	TBVector	zeroVec;

	if(offset == NULL)
	{
		SETVECTOR(zeroVec, 0.0f, 0.0f, 0.0f, 0.0f);
		offset=zeroVec;
	}

	bmVectorCopy(cameraInfo->lookAtOffset, offset);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraPositionDestination
   Purpose : Sets the destination for the camera position to move to
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraPositionDestination(CAMERAINFO *cameraInfo, TBVector pos)
{
	bmVectorCopy(cameraInfo->destinationPos,pos);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtDestination
   Purpose : Sets the destination for the camera look at to move to
   Parameters : CAMERAINFO, TBVector position
   Returns : 
   Info : 
*/
void SetCameraLookAtDestination(CAMERAINFO *cameraInfo, TBVector pos)
{
	bmVectorCopy(cameraInfo->destinationLookAt,pos);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtFollowActorInstance
   Purpose : Sets the destination for the camera look at to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/

void SetCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo, ACTORINSTANCE *ourActorInstance, TBActorInstance *actorInstance, TBActorNodeInstance *node)
{
	cameraInfo->ourActorInstanceLookAt = ourActorInstance;

	if(actorInstance)
		cameraInfo->actorInstanceLookAt = actorInstance;

	cameraInfo->actorInstanceLookAtOffset[X] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Y] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Z] = 0.0f;

	cameraInfo->actorInstanceNodeLookAt = node;// CPW: baFindNode(ROOTNODE(actorInstance->rootNodeInstance),node);
}

/*
void SetCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo, TBActorInstance *actorInstance, char *node)
{
	cameraInfo->actorInstanceLookAt = actorInstance;
	cameraInfo->actorInstanceLookAtOffset[X] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Y] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Z] = 0.0f;
	if (node)
	{
		cameraInfo->actorInstanceNodeLookAt = baFindNode(ROOTNODE(actorInstance->rootNodeInstance),node);
	}
}
*/

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtFollowActorInstanceNode
   Purpose : Sets the destination for the camera look at to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/

void SetCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo, ACTORINSTANCE *ourActorInstance, TBActorInstance *actorInstance, char *node)
{
	TBActorNodeInstance *pNode = NULL;

	if(node)
		pNode = baFindNode(ROOTNODE(actorInstance->rootNodeInstance),node);

	SetCameraLookAtFollowActorInstance(cameraInfo, ourActorInstance, actorInstance, pNode);
}

/*
	cameraInfo->actorInstanceLookAt = actorInstance;
	cameraInfo->actorInstanceLookAtOffset[X] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Y] = 0.0f;
	cameraInfo->actorInstanceLookAtOffset[Z] = 0.0f;
	if (node)
	{
		cameraInfo->actorInstanceNodeLookAt = node;
	}
*/

/* --------------------------------------------------------------------------------
   Function : DisableCameraLookAtFollowActorInstance
   Purpose : disables the look at follow instance mode
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraLookAtFollowActorInstance(CAMERAINFO *cameraInfo)
{
	cameraInfo->actorInstanceLookAt = NULL;
}

/* --------------------------------------------------------------------------------
   Function : SetCameraLookAtActorInstanceOffset
   Purpose : Sets the offset from the center of the actor instance to point the camera at
   Parameters : CAMERAINFO, TBVector offset
   Returns : 
   Info : 
*/
void SetCameraLookAtActorInstanceOffset(CAMERAINFO *cameraInfo, TBVector offset)
{
	bmVectorCopy(cameraInfo->actorInstanceLookAtOffset,offset);
}

/* --------------------------------------------------------------------------------
   Function : SetCameraPosFollowActorInstance
   Purpose : Sets the destination for the camera pos to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraPosFollowActorInstance(CAMERAINFO *cameraInfo, TBActorInstance *actorInstance, char *node)
{
	cameraInfo->actorInstancePos = actorInstance;
	if (node)
	{
		cameraInfo->actorInstanceNodePos = baFindNode(ROOTNODE(actorInstance->rootNodeInstance),node);
	}
}

/* --------------------------------------------------------------------------------
   Function : SetCameraPosFollowActorInstance
   Purpose : Sets the destination for the camera pos to an actor instance
   Parameters : CAMERAINFO, TBActorInstance, node or NULL
   Returns : 
   Info : 
*/
void SetCameraPosFollowActorInstanceNode(CAMERAINFO *cameraInfo, TBActorInstance *actorInstance, TBActorNodeInstance *node)
{
	cameraInfo->actorInstancePos = actorInstance;
	if (node)
	{
		cameraInfo->actorInstanceNodePos = node;
	}
}

/* --------------------------------------------------------------------------------
   Function : RotateCameraAroundLookAt
   Purpose : Rotates the camera in the Y and X rotations around the lookat point
   Parameters : CAMERAINFO, TBActorInstance
   Returns : 
   Info : 
*/
void RotateCameraAroundLookAt(CAMERAINFO *cameraInfo, float yAngle,float xAngle)
{
	TBVector temp,temp2;

	bmVectorSub(temp,cameraInfo->destinationPos,cameraInfo->lookAt);
	bmVectorRotateY(temp2, temp, yAngle);
	bmVectorAdd(cameraInfo->destinationPos,cameraInfo->lookAt,temp2);
}


/* --------------------------------------------------------------------------------
   Function : ConvertFromCameraToGlobalRotation
   Purpose : Converts an angle from camera relative to global
   Parameters : CAMERAINFO, angle
   Returns : global angle
   Info : 
*/
float ConvertFromCameraToGlobalRotation(CAMERAINFO *cameraInfo,float angle)
{
	float	cameraAngleY;

	// PP: if we're using the Taz Cam, then camera rotation IS global rotation
/*	if(CheatActive(CHEAT_TAZCAM))
	{
		return Aabs(TWOPI-angle);
	}*/

	if ((cameraInfo->cameraCase)&&((cameraInfo->cameraCase->time > 1.0f) || (!(cameraInfo->cameraCase->flags&CAMERACASE_TRANSITIONCONTROL))))
	{
		cameraAngleY = Aabs((float)bmATan2((cameraInfo->cameraCase->camera->lookAt[X]-cameraInfo->cameraCase->camera->pos[X]), (cameraInfo->cameraCase->camera->lookAt[Z]-cameraInfo->cameraCase->camera->pos[Z])));
	}
	else
	{
		cameraAngleY = Aabs((float)bmATan2((cameraInfo->lookAt[X]-cameraInfo->pos[X]), (cameraInfo->lookAt[Z]-cameraInfo->pos[Z])));
	}
	return (Aabs(cameraAngleY+angle));
}

/* --------------------------------------------------------------------------------
   Function : SetFixedDistanceToLookAt
   Purpose : Sets the maximum and minimum distance between the camera and the lookat
   Parameters : CAMERAINFO, float distance
   Returns : 
   Info : 
*/
void SetFixedDistanceToLookAt(CAMERAINFO *cameraInfo, float distance)
{
	cameraInfo->fixedDistanceToLookAt = distance;
}

/* --------------------------------------------------------------------------------
   Function : EnableCameraAngleAboveHorizontal
   Purpose : Sets the optimum angle above horizontal for the camera
   Parameters : CAMERAINFO, angle in rads above horizontal
   Returns : 
   Info : 
*/
void EnableCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo, float angle)
{
	cameraInfo->angleAboveHorizontal = angle;
	cameraInfo->flags |= CAMERA_ANGLEOVERRIDEY;
}

/* --------------------------------------------------------------------------------
   Function : SetDefaultCameraAngleAboveHorizontal
   Purpose : Sets the default angle above horizontal for the camera
   Parameters : CAMERAINFO, angle in rads above horizontal
   Returns : 
   Info : 
*/
void SetDefaultCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo, float angle)
{
	cameraInfo->defaultAngleAboveHorizontal = angle;
}

/* --------------------------------------------------------------------------------
   Function : EnableCameraRotateBehind
   Purpose : Sets the camera to rotate behind the actorlookat, (only works with SetCameraLookAtFollowActorInstance)
   Parameters : CAMERAINFO, max rads to rotate in one second
   Returns : 
   Info : 
*/
void EnableCameraRotateBehind(CAMERAINFO *cameraInfo, float radsPerSecond)
{
	cameraInfo->flags |= CAMERA_ROTATEBEHIND;
	cameraInfo->rotRadsPerSecond = radsPerSecond;
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraRotateBehind
   Purpose : Disable the rotate behind code
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraRotateBehind(CAMERAINFO *cameraInfo)
{
	cameraInfo->flags &= ~CAMERA_ROTATEBEHIND;
}

/* --------------------------------------------------------------------------------
   Function : EnableCameraFollowBehind
   Purpose : Sets the camera to follow behind the actorlookat
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void EnableCameraFollowBehind(CAMERAINFO *cameraInfo, ACTORINSTANCE *pActorInstance)
{
	cameraInfo->flags |= CAMERA_FOLLOWBEHIND;
	cameraInfo->ourActorInstanceLookAt = pActorInstance;
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraFollowBehind
   Purpose : 
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraFollowBehind(CAMERAINFO *cameraInfo)
{
	cameraInfo->flags &= ~CAMERA_FOLLOWBEHIND;
// CPW: 	cameraInfo->ourActorInstanceLookAt = NULL;
}

/* --------------------------------------------------------------------------------
   Function : EnableCameraBigFall
   Purpose : Sets the camera to follow a big fall
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void EnableCameraBigFall(CAMERAINFO *cameraInfo, ACTORINSTANCE *pActorInstance)
{
	// PP: NULL actorInstance check
	if(cameraInfo->actorInstanceLookAt != NULL)
	{
		bkPrintf(" *** ENabling CAMERA_BIGFALL ***\n");

		cameraInfo->flags |= CAMERA_BIGFALL;
		cameraInfo->ourActorInstanceLookAt = pActorInstance;
		cameraInfo->bigfall.stopTime = -1;
		bmVectorCopy(cameraInfo->bigfall.startPos, cameraInfo->actorInstanceLookAt->position);
	}
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraBigFall
   Purpose : 
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/

#define CAMERA_BIGFALL_STOPDELAY (1.0f)

void DisableCameraBigFall(CAMERAINFO *cameraInfo)
{
	bkPrintf(" *** DISabling CAMERA_BIGFALL ***\n");
	cameraInfo->bigfall.stopTime = gameStatus.gameTime + CAMERA_BIGFALL_STOPDELAY;
}


/* --------------------------------------------------------------------------------
   Function : EnableCameraCatchup
   Purpose : 
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void EnableCameraCatchup(CAMERAINFO *cameraInfo, ACTORINSTANCE *pActorInstance)
{
	cameraInfo->flags |= CAMERA_CATCHUP;
	cameraInfo->ourActorInstanceLookAt = pActorInstance;
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraCatchup
   Purpose : 
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraCatchup(CAMERAINFO *cameraInfo)
{
	cameraInfo->flags &= ~CAMERA_CATCHUP;
// CPW: 	cameraInfo->ourActorInstanceLookAt = NULL;
}


/* --------------------------------------------------------------------------------
   Function : EnableCameraChaseCam
   Purpose : Sets the camera to rotate behind the actor velocity, (only works with SetCameraLookAtFollowActorInstance)
   Parameters : CAMERAINFO, max rads to rotate in one second
   Returns : 
   Info : 
*/
void EnableCameraChaseCam(CAMERAINFO *cameraInfo, float radsPerSecond, TBVector vel)
{
	cameraInfo->chaseCamVel = vel;
	cameraInfo->flags |= CAMERA_CHASECAM;
	cameraInfo->rotRadsPerSecond = radsPerSecond;
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraChaseCam
   Purpose : Disable the chase came code
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraChaseCam(CAMERAINFO *cameraInfo)
{
	cameraInfo->flags &= ~CAMERA_CHASECAM;
}

/* --------------------------------------------------------------------------------
   Function : DisableCameraAngleAboveHorizontal
   Purpose : Disables the angle override for camera height
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void DisableCameraAngleAboveHorizontal(CAMERAINFO *cameraInfo)
{
	cameraInfo->flags &= ~CAMERA_ANGLEOVERRIDEY;
}

/* --------------------------------------------------------------------------------
   Function : SetCameraCollision
   Purpose : Sets camera collision on or off
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void SetCameraCollision(CAMERAINFO *cameraInfo, int collide)
{
	if(collide)
		cameraInfo->flags &= ~CAMERA_NOCOLLISION;
	else
		cameraInfo->flags |= CAMERA_NOCOLLISION;

/*
#ifdef CHRIS
	if(collide)
		bkPrintf("Kamera Kollide ON\n");
	else
		bkPrintf("Kamera Kollide OFF\n");
#endif
*/
}		

/* --------------------------------------------------------------------------------
   Function : SetCameraSmooth
   Purpose : Set camera smooth values for the camera and lookat points
   Parameters : CAMERAINFO, float posSmooth, float lookAtSmooth
   Returns : 
   Info : Controls how fast the camera and lookAt positions move to their destination positions
			0 - no move, 1 - move entire distance in one update
*/
void SetCameraSmooth(CAMERAINFO *cameraInfo, float posSmooth, float lookAtSmooth)
{
	cameraInfo->posSmooth = posSmooth;
	cameraInfo->lookAtSmooth = lookAtSmooth;
}

/* --------------------------------------------------------------------------------
   Function : SaveCameraSettings
   Purpose : Save all camera settings except current camera pos and current camera lookAt pos
   Parameters : dest,source
   Returns : 
   Info : 
*/
void SaveCameraSettings(CAMERAINFO *dest, CAMERAINFO *source)
{
	fmemcpy(dest,source,sizeof(CAMERAINFO));
}

/* --------------------------------------------------------------------------------
   Function : RestoreCameraSettings
   Purpose : restore all camera settings except pos and lookAt pos
   Parameters : dest,source
   Returns : 
   Info : 
*/
void RestoreCameraSettings(CAMERAINFO *dest, CAMERAINFO *source, bool restorePos, bool restoreFade)
{
	TBVector	pos;
	TBVector	lookAt;
	FADE		tempFade;

	if (restorePos==false)
	{
		bmVectorCopy(pos,dest->pos);
		bmVectorCopy(lookAt,dest->lookAt);
	}
	if (restoreFade==false)
	{
		fmemcpy(&tempFade, &dest->fade, sizeof(FADE));
	}

	fmemcpy(dest,source,sizeof(CAMERAINFO));

	if (restoreFade==false)
	{
		fmemcpy(&dest->fade, &tempFade, sizeof(FADE));
	}
	if (restorePos==false)
	{
		bmVectorCopy(dest->pos,pos);
		bmVectorCopy(dest->lookAt,lookAt);
	}
}

/* --------------------------------------------------------------------------------
   Function : SetCameraCase
   Purpose : Set the current camera override
   Parameters : dest,source
   Returns : 
   Info : the override camera will take over for standard cam although both will be updated
*/
void SetCameraCase(CAMERAINFO *camera,CAMERACASE *cameraCase)
{
	camera->cameraCase = cameraCase;
}

/*  --------------------------------------------------------------------------------
	Function	: RespondToCameraControl
	Purpose		: Adjust the camera based on the control input
	Parameters	: player
	Returns		: 
	Info		: 
*/
void RespondToCameraControl(PLAYER *player)
{
	PADCONTROLSTATUS	*controller = player->controller;
	CAMERAINFO			*cameraInfo = player->camera;
	float				tempFloat;

	// PP: don't respond to anything if the pad control flag is low
	if(!(cameraInfo->flags & CAMERA_PADCONTROL))
	{
		return;
	}

	// update camera rotate etc
	if (controller->l1Channel->value)
	{
		// CMD: not if in mechatweety boss game
		switch(map.sceneNumber)
		{
		case SCENE_TAZBOSS:
			break;
		default:
			RotateCameraAroundLookAt(cameraInfo,1*(PI/fps),0.0f);
			break;
		}
	}
	if (controller->r1Channel->value)
	{
		// CMD: not if in mechatweety boss game
		switch(map.sceneNumber)
		{
		case SCENE_TAZBOSS:
			break;
		default:
			RotateCameraAroundLookAt(cameraInfo,-1*(PI/fps),0.0f);
			break;
		}
	}

	if (cameraInfo->flags&CAMERA_LOOKAROUND)
	{
		SetFixedDistanceToLookAt(cameraInfo,METERTOUNIT(0.1f));
		if (controller->mag1)
		{
			// TP: rotate around y
			RotateCameraAroundLookAt(cameraInfo,controller->x1*(PI/fps),0.0f);

			// TP: control angle above horizontal
			tempFloat = (-controller->y1*(PI/2/fps)) + cameraInfo->angleAboveHorizontal;
			if (tempFloat > RAD(80)) tempFloat = RAD(80);
			if (tempFloat < RAD(-80)) tempFloat = RAD(-80);
			EnableCameraAngleAboveHorizontal(cameraInfo,tempFloat);
		}
	}
	else
	{
		if(cameraInfo->flags&CAMERA_ROPECONTROL)
		{
			if(controller->mag2)
			{
				RotateCameraAroundLookAt(cameraInfo,-controller->x2*(PI/fps),controller->y2*(PI/fps));
			}
		}
		else
		{
			SetFixedDistanceToLookAt(cameraInfo,cameraInfo->defaultFixedDistance);
			// check for look around mode

			if (controller->mag2)
			{
				// CMD: don't want thjis in mechatweety fight
				switch(map.sceneNumber)
				{
				case SCENE_TAZBOSS:
					break;
				default:
					// TP: standard look around mode

					// TP: rotate around y
					RotateCameraAroundLookAt(cameraInfo,-controller->x2*(PI/fps),controller->y2*(PI/fps));
					// TP: control angle above horizontal
					EnableCameraAngleAboveHorizontal(cameraInfo,cameraInfo->defaultAngleAboveHorizontal+((-controller->y2)*RAD(30.0f)));
					break;
				}
			}
			else
			{
				EnableCameraAngleAboveHorizontal(cameraInfo,cameraInfo->defaultAngleAboveHorizontal);
			}
		}
	}
}

void ResetCamera(CAMERAINFO *pCameraInfo)
{
	// CPW: force camera to be looking at actor at a sensible position
	float dist, height;

	TBVector	tempVector;

	DisableCameraCatchup(pCameraInfo);

	SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
	bmVectorSet(tempVector,0.0f,0.0f,METERTOUNIT(2.0f),1.0f);
	VectorQuatMultiply(tempVector,pCameraInfo->ourActorInstanceLookAt->actorInstance.orientation);
	bmVectorAdd(tempVector,tempVector,pCameraInfo->ourActorInstanceLookAt->actorInstance.position);

	SetCameraPosition(pCameraInfo, tempVector);//cameraInfo.resetPoint
	SetCameraLookAtPosition(pCameraInfo,pCameraInfo->ourActorInstanceLookAt->actorInstance.position);

	bmVectorCopy(pCameraInfo->destinationPos, pCameraInfo->pos);

	dist = METERTOUNIT(1.0f);
	height = bmTan(pCameraInfo->angleAboveHorizontal) * dist;
	pCameraInfo->destinationPos[Y] = pCameraInfo->destinationLookAt[Y] + height;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FLY CAM
										
							Handy debug camera that can be flown freely around the environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: TEMP
#include "shadow.h"		// PP: functions to create and draw shadows on the landscape


CAMERAINFO				flyCam;					// PP: the debug camera that can be flown around freely
bool					flyCamActive=false;		// PP: is the fly cam active
extern TBGuiMenuItem*	guiFlyCam;


/*	--------------------------------------------------------------------------------
	Function 	: initFlyCam
	Purpose 	: init fly cam
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initFlyCam(void)
{
	
}


/*	--------------------------------------------------------------------------------
	Function 	: setFlyCam
	Purpose 	: set the fly cam on/off
	Parameters 	: true:on, false:off
	Returns 	: 
	Info 		: 
*/
void setFlyCam(const bool on)
{
	flyCamActive=on;

	if(flyCamActive)
	{
		// PP: copy current camera's details into fly cam...

		/*if(lastSentCameraCopy.cameraCase != NULL)
		{
			flyCam= *(lastSentCameraCopy.cameraCase->camera);
		}
		else*/
		{
			flyCam=lastSentCameraCopy;
		}

		// PP: remove the control lock applied by navigable books
		gameStatus.navigatingMenus=false;
	}

	// PP: enable/disable taz controls
	gameStatus.controlSuspend=flyCamActive;

	// PP: set/clear gui menu item tick
	if(guiFlyCam != NULL)
	{
		bgSetMenuItem(guiFlyCam,NULL,NULL,NULL,on?BGUIMENUITEMFLAG_TICKED:0,BGUIMENUITEMFLAG_TICKED);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: toggleFlyCam
	Purpose 	: toggle the fly cam on/off
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void toggleFlyCam(void)
{
	setFlyCam(flyCamActive^1);
}


/*	--------------------------------------------------------------------------------
	Function 	: updateFlyCam
	Purpose 	: update the fly cam
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define FLYCAM_MOVE_STEP	2000.0f
#define FLYCAM_TILT_STEP	2.0f

#define FLYCAM_MOVE_FAST_FACTOR		5.0f

void updateFlyCam(void)
{
	TBVector	movement;

	// PP: ALL THAT FOLLOWS IS PAD CONTROL - NOT DONE WHILE NAVIGATING MENUS
	if(gameStatus.navigatingMenus) return;

	bmVectorZero(movement);

	// PP: strafe
	movement[X]= controller1.x2;

	// PP: forward/backward
	movement[Z]= -controller1.y2;

	// PP: up/down
	if(controller1.r3Channel->value) movement[Y]+=1.0f;
	if(controller1.l3Channel->value) movement[Y]=-1.0f;

	bmVectorCMul(movement, movement, 1.0f+(controller1.r2*FLYCAM_MOVE_FAST_FACTOR));
	bmVectorCMul(movement, movement, 1.0f-controller1.l2);

	bmVectorCMul(movement, movement, FLYCAM_MOVE_STEP);
	bmVectorCMul(movement, movement, rawFTime);

	// PP: tilt

	static TBVector2D	ang={0,0};
	TBVector			lookAt;

	ang[X]-=controller1.y1*FLYCAM_TILT_STEP*rawFTime;
	ang[Y]+=controller1.x1*FLYCAM_TILT_STEP*rawFTime;

	ang[X]=CONSTRAIN(ang[X], -1.5f, 1.5f);
	ang[Y]=Aabs(ang[Y]);

	// PP: apply movement...

	TBVector	rotatedMovement;

	bmVectorRotateXY(rotatedMovement, movement, ang[X], ang[Y]);

	bmVectorAdd(flyCam.pos, flyCam.pos, rotatedMovement);

	// PP: lookat

	SETVECTOR(lookAt, 0,0,1000,0);

	bmVectorRotateXY(flyCam.lookAt, lookAt, ang[X], ang[Y]);

	bmVectorAdd(flyCam.lookAt, flyCam.lookAt, flyCam.pos);

	if(!CheatActive(CHEAT_TIMEFREEZE))// PP: can't do these things in timefreeze mode
	{
		// PP: cross button places taz where camera is, and switches back to normal camera

		if(CANCELBUTTON)
		{
			SetActorPosition(gameStatus.player1, flyCam.pos);
			SetCameraPosition(&camera[0], flyCam.pos);

			trailReset(gameStatus.player1, TRAIL_RESET_ALL);

			// PP: pourquoi pas
			PlaySample("balloonpop1.wav");

			setFlyCam(false);
		}

		// PP: shadow light test on square (point) and circle (directional)

		if(controller1.squareDebounceChannel->value)
		{
			// PP: clear all shadow lights
			shadowLights.init();

			// PP: add a shadow light at the camera position
			selectShadowLight(addShadowLight(flyCam.pos, SHADOWLIGHTTYPE_POINT, ZVEC, SHADOWLIGHT_DEFAULT_COLOUR, SHADOWLIGHT_DEFAULT_SHADOW_DARKNESS, SHADOWLIGHT_INFINITE_RANGE, SHADOWLIGHT_DEFAULT_PLAYER_FLAGS));
		}

		if(controller1.circleDebounceChannel->value)
		{
			// PP: clear all shadow lights
			shadowLights.init();

			// PP: add a shadow light at the camera position
			selectShadowLight(addShadowLight(flyCam.pos, SHADOWLIGHTTYPE_DIRECTIONAL, VEC(ang[X], ang[Y]), SHADOWLIGHT_DEFAULT_COLOUR, SHADOWLIGHT_DEFAULT_SHADOW_DARKNESS, SHADOWLIGHT_INFINITE_RANGE, SHADOWLIGHT_DEFAULT_PLAYER_FLAGS));
		}
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FiRST-PERSON MODE

										Basically attaches the camera to Taz's heed.
										
								This should be interesting, especially in stereoscopic mode.
				It'd be cool to award this and the stereoscopic mode together as cheats when you complete the game.
									It lets you see the whole game from Taz's perspective.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


bool			firstPersonMode=false;
CAMERAINFO		firstPersonCam;


/*	--------------------------------------------------------------------------------
	Function 	: updateFirstPersonMode
	Purpose 	: update for first-person mode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void updateFirstPersonMode(void)
{
	// PP: first things first I suppose, get the position and orientation of Taz's head

	// PP: mark it for just now so's we can see...

	TBVector				camPos;

	TBActorNodeInstance*	camBone;

	camBone=baFindNode(gameStatus.player1->actorInstance.rootNodeInstance,"snoutstem");

	if(camBone != NULL)
	{
		baGetNodesWorldPosition(&(gameStatus.player1->actorInstance), camBone, camPos);
	}

	SetCameraPosition(&firstPersonCam, camPos);
}


/*	--------------------------------------------------------------------------------
	Function 	: drawFirstPersonMode
	Purpose 	: draw for first-person mode
	Parameters 	: 
	Returns 	: 
	Info 		: probably just for early debug?  dunno yet.
*/
void drawFirstPersonMode(void)
{
	bdSetIdentityObjectMatrix();
	utilDrawSphere(firstPersonCam.pos, 100.0f, GREEN);
}


/*	--------------------------------------------------------------------------------
	Function 	: updateFirstPersonMode
	Purpose 	: switch first-person mode on/off
	Parameters 	: true=on/ false=off
	Returns 	: 
	Info 		: 
*/
void setFirstPersonMode(const bool on)
{
	firstPersonMode=on;
}


/*	--------------------------------------------------------------------------------
	Function 	: toggleFirstPersonMode
	Purpose 	: toggle first-person mode on/off
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void toggleFirstPersonMode(void)
{
	setFirstPersonMode(!firstPersonMode);
}


/*  --------------------------------------------------------------------------------
	Function	: ProcessCameraEnterLookAround
	Purpose		: Called each frame while the camera enters the lookaround mode
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/

void ProcessCameraEnterLookAround(PLAYER *player)
{
	ACTORINSTANCE		*actorInstance = player->actorInstance;
	CAMERAINFO			*cameraInfo = player->camera;
	PADCONTROLSTATUS	*controller = player->controller;

	// TP: lookaround mode
	if (RespondToStateExit(actorInstance)) 
	{
		SetCameraSmooth(player->camera, defaultPosSmooth, defaultLookatSmooth);
		player->camera->flags &= ~CAMERA_LOOKAROUND;
		player->camera->flags |= CAMERA_SMARTCAM;
		SetCameraCollision(player->camera, true);
	}

	// TP: check for exiting look around mode
	if (controller->r2Channel->value == 0.0f)
	{
		SetCameraSmooth(player->camera, defaultPosSmooth, defaultLookatSmooth);
		player->camera->flags &= ~CAMERA_LOOKAROUND;
		player->camera->flags |= CAMERA_SMARTCAM;
		SetCameraCollision(player->camera, true);

		ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		//-----------------STATE CHANGE-----------------
	}

	TBVector tempVector;
	bmVectorSub(tempVector, player->camera->pos, player->camera->lookAt);
	if (bmVectorLen(tempVector) < METERTOUNIT(0.5f))
	{
		SetCameraSmooth(player->camera, defaultPosSmooth, defaultLookatSmooth);
		SetCameraCollision(player->camera, false);
		player->camera->flags |= CAMERA_SMARTCAM;
		player->camera->flags |= CAMERA_BLINKEYES;
		
		if (!player->actorInstance->characterInfo->blinkEyes)
		{
			player->actorInstance->characterInfo->blinkEyes = new BLINKEYES;
		}
		player->camera->flags |= CAMERA_NOCOLLISION;
		gameStatus.chrisCam = FALSE;
		ChangeState(actorInstance,STATE_LOOKAROUND);
		player->camera->fade.StartFadeUp(FADETYPE_NORMAL, 0.5f);
		player->actorInstance->flags |= ACTORFLAG_DONTDRAW;

		// TP: match camera to direction taz is looking
		TBVector	temp,temp2;
		ZEROVECTOR(temp);
		temp[Z] -= METERTOUNIT(0.1f);
		bmVectorRotateY(temp2, temp, Aabs(-GetAngleAroundYFromQuat(player->actorInstance->actorInstance.orientation)+PI));
		bmVectorAdd(cameraInfo->pos,cameraInfo->lookAt,temp2);
		bmVectorCopy(cameraInfo->destinationPos,cameraInfo->pos);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ProcessCameraLookAround
	Purpose		: Called each frame while the lookaround mode is active
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/

void ProcessCameraLookAround(PLAYER *player)
{
	ACTORINSTANCE	*actorInstance = player->actorInstance;
	CAMERAINFO		*cameraInfo = player->camera;
	PADCONTROLSTATUS	*controller = player->controller;

	// TP: turn taz to facing of camera
	TBQuaternion	tempQuat;
	bmQuatYRotation(tempQuat,Aabs(-(player->camera->cameraState.cameraAngles[Y]+PI)));
	ActorRotate(actorInstance,tempQuat,0.0f);
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);

	// TP: test for spit
	if (controller->triangleDebounceChannel->value)
	{
		if (actorInstance->characterInfo->inMouth)
		{
			TazSpit(actorInstance);
		}
	}

	// TP: if costume is swat Taz then the square key should shoot cheese
	if ((controller->squareDebounceChannel->value)&&(actorInstance->characterInfo->costume==COSTUME_SWAT))
	{
		char	temp[256];
		sprintf(temp,"%X %X cheesegun",gameStatus.player[0].actorInstance,gameStatus.player[0].actorInstance);
		bkGenerateEvent("special",temp);
	}


	// TP: check for exiting look around mode
	if (controller->r2Channel->value == 0.0f)
	{
		// CMD: sort out prompts
		if((americanEvents)&&(americanEvents->book))
		{
			if(americanEvents->localFlags & MORRON_BOOKOPEN)
			{
				americanEvents->book->open();
			}
		}
		// TP: sort out flags
		player->actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
		player->camera->flags &= ~(CAMERA_LOOKAROUND|CAMERA_BLINKEYES|CAMERA_NOCOLLISION);
		gameStatus.chrisCam = TRUE;
		
		// TP: take care of blinking eyes
		if (player->actorInstance->characterInfo->blinkEyes)
		{
			SAFE_DELETE(player->actorInstance->characterInfo->blinkEyes);
		}

		ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		//-----------------STATE CHANGE-----------------
	}
}