// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : quakecam.cpp
//   Purpose : provides functions to shake camera
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "camera.h"
#include "cameracase.h"				// PP: camera case stuff

#include "maths.h"
#include "quakecam.h"


/* --------------------------------------------------------------------------------
   Function : InitCameraOffsets
   Purpose : initialises camera offsets
   Parameters : CAMERAINFO *
   Returns : 
   Info : // PP: 
*/
void InitCameraOffsets(CAMERAINFO* const cameraInfo)
{
	SETVECTOR(cameraInfo->posOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	SETVECTOR(cameraInfo->lookAtOffset, 0.0f, 0.0f, 0.0f, 0.0f);
	cameraInfo->tiltOffset = 0.0f;
}


/* --------------------------------------------------------------------------------
   Function : UpdateQuakeCam
   Purpose : controlls camera when earthquake effect is required
   Parameters : CAMERAINFO *
   Returns : 
   Info : 
*/

void UpdateQuakeCam(CAMERAINFO *cameraInfo)
{	
	TBVector		tempPos;
	TBVector		tempLookAt;
	float			tempTilt;
	int				counter;

	if(cameraInfo->flags & CAMERA_QUAKE)
	{
		//shake camera pos
		for(counter = 0; counter < 3; counter++)
		{
			tempPos[counter] = (float)(bmRand()%250) * CAMERA_MAX_QUAKE;
			tempLookAt[counter] = (float)(bmRand()%250) * CAMERA_MAX_QUAKE;
		}
		CheckQuakeVectorForOpposites(cameraInfo->posOffset, tempPos);
		CheckQuakeVectorForOpposites(cameraInfo->lookAtOffset, tempLookAt);
		//shake tilt
		tempTilt = (float)(bmRand()%100)/100.0f*CAMERA_QUAKE_TILT;
		tempTilt = CheckQuakeTiltForOpposites(cameraInfo->tiltOffset, tempTilt);

		cameraInfo->quakeOffsetLevel = CAMERA_MAX_QUAKE;
		cameraInfo->tiltOffsetLevel = CAMERA_QUAKE_TILT;
	}

	else
	if(cameraInfo->flags & CAMERA_QUAKEFADE)
	{
		cameraInfo->quakeOffsetLevel -= QUAKE_FADE_RATE*fTime;
		cameraInfo->tiltOffsetLevel -= QUAKE_FADE_RATE*fTime;
		if(cameraInfo->quakeOffsetLevel < 0.0f)
		{
			SETVECTOR(tempPos, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempLookAt, 0.0f, 0.0f, 0.0f, 1.0f);
			tempTilt = 0.0f;
			// TP: might need this?
			cameraInfo->tiltOffset = 0.0f;
			cameraInfo->tiltOffsetLevel = 0.0f;

			cameraInfo->flags &= ~CAMERA_QUAKEFADE;
		}
		if(cameraInfo->tiltOffsetLevel < 0.0f)
		{
			cameraInfo->tiltOffsetLevel = 0.0f;
		}
		//shake camera pos
		for(counter = 0; counter < 3; counter++)
		{
			tempPos[counter] = (float)(bmRand()%250) * cameraInfo->quakeOffsetLevel;
			tempLookAt[counter] = (float)(bmRand()%250) * cameraInfo->quakeOffsetLevel;
		}
		CheckQuakeVectorForOpposites(cameraInfo->posOffset, tempPos);
		CheckQuakeVectorForOpposites(cameraInfo->lookAtOffset, tempLookAt);
		//shake tilt
		if(tempTilt = (float)(bmRand()%100)/100.0f*cameraInfo->tiltOffsetLevel)
		{
			tempTilt = CheckQuakeTiltForOpposites(cameraInfo->tiltOffset, tempTilt);
		}
	}

	else
	if(cameraInfo->flags & CAMERA_QUAKEBANG)
	{
		//shake camera pos
		for(counter = 0; counter < 3; counter++)
		{
			tempPos[counter] = (float)(bmRand()%250) * CAMERA_MAX_QUAKEBANG;
			tempLookAt[counter] = (float)(bmRand()%250) * CAMERA_MAX_QUAKEBANG;
		}
		CheckQuakeVectorForOpposites(cameraInfo->posOffset, tempPos);
		CheckQuakeVectorForOpposites(cameraInfo->lookAtOffset, tempLookAt);
		//shake tilt
		tempTilt = (float)(bmRand()%100)/100.0f*CAMERA_QUAKE_TILT;
		tempTilt = CheckQuakeTiltForOpposites(cameraInfo->tiltOffset, tempTilt);

		cameraInfo->quakeOffsetLevel = CAMERA_MAX_QUAKEBANG;
		cameraInfo->tiltOffsetLevel = CAMERA_QUAKE_TILT;
	}

	else
	{
		SETVECTOR(tempPos, 0.0f, 0.0f, 0.0f, 1.0f);
		SETVECTOR(tempLookAt, 0.0f, 0.0f, 0.0f, 1.0f);
		tempTilt = 0.0f;

		cameraInfo->quakeOffsetLevel = 0.0f;
		cameraInfo->tiltOffsetLevel = 0.0f;
	}

	// PP: add new offsets into camerainfo
	bmVectorAdd(cameraInfo->posOffset, cameraInfo->posOffset, tempPos);
	bmVectorAdd(cameraInfo->lookAtOffset, cameraInfo->lookAtOffset, tempLookAt);
	cameraInfo->tiltOffset += tempTilt;
}

/* --------------------------------------------------------------------------------
   Function : CheckQuakeVectorForOpposites
   Purpose : check where offset was last frame, and correct offset so that new offset is on other side of origin
   Parameters : TBVector *2
   Returns : 
   Info : 
*/

void CheckQuakeVectorForOpposites(TBVector old, TBVector current)
{
	int		counter;

	for(counter = 2; counter >= 0; --counter)
	{
		if((old[counter] > 0.0f) && (current[counter] > 0.0f))
		{
			current[counter] = -current[counter];
		}
		if((old[counter] <= 0.0f) && (current[counter] <= 0.0f))
		{
			current[counter] = -current[counter];
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CheckQuakeTiltForOpposites
   Purpose : same as above, but for a float
   Parameters : TBVector *2
   Returns : 
   Info : 
*/

float CheckQuakeTiltForOpposites(float old, float current)
{
	if((old > 0.0f) && (current > 0.0f))
	{
		current = -current;
	}
	if((old <= 0.0f) && (current <= 0.0f))
	{
		current = -current;
	}
	return current;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												'RUMBLE' STUFF
											for want of a better name
  
											Smooth camera shake effects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

float rumblePosReturnSpeed		=15.0f;			// PP: TODO: comment
float rumblePosDamp				=0.2f;			// PP: TODO: comment

float rumbleLookAtReturnSpeed	=15.0f;			// PP: TODO: comment
float rumbleLookAtDamp			=0.2f;			// PP: TODO: comment

float rumbleAngReturnSpeed		=12.0f;// PP: 7.0f;
float rumbleAngDamp				=4.5f;


/* --------------------------------------------------------------------------------
   Function : updateOffsetRumble
   Purpose : updates a rumbling offset
   Parameters : offset to rumble, offset velocity, return speed, damping value
   Returns : 
   Info : 
*/
static void updateOffsetRumble(TBVector offset, TBVector velocity, const float returnSpeed, const float damping)
{
	TBVector	tempVec;

	// PP: check we're not wasting our time...

	if((velocity[X] == 0.0f)&&(velocity[Y] == 0.0f)&&(velocity[Z] == 0.0f))
	{
		if((offset[X] == 0.0f)&&(offset[Y] == 0.0f)&&(offset[Z] == 0.0f))
		{
			return;
		}
	}

	// PP: spring back to home pos...

	TBVector	returnVel;

	bmVectorCMul(returnVel, offset, -returnSpeed);
	bmVectorAdd(velocity, velocity, returnVel);

	// PP: correct velocity to take framerate *AND DECELERATION OVER THE FRAME* into account...

	float		distance;// PP: distance to be traveled this frame
	float		speed;// PP: initial speed

	// PP: get length of velocity
	speed=bmVectorLen(velocity);

	// PP: get distance to be traveled this frame // PP: s=ut+½at²
	distance=(speed*fTime)+(0.5f*(-damping)*SQR(fTime));

	if(distance > 0.0f)
	{
		// PP: re-form the velocity vector with its new length
		bmVectorScaleToLength(tempVec, velocity, distance);

		// PP: apply velocity to offset
		bmVectorAdd(offset, offset, tempVec);
	}
	
	// PP: lose some energy due to inelasticity...

	speed-=(damping/fps);

	if(speed>0.0f)
	{
		bmVectorScaleToLength(velocity, velocity, speed);
	}
	else
	{
		bmVectorZero(velocity);
	}
}


/* --------------------------------------------------------------------------------
   Function : UpdateRumbleCam
   Purpose : updates rumble effects on a camera
   Parameters : CAMERAINFO *
   Returns : 
   Info : 
*/
void UpdateRumbleCam(CAMERAINFO *cameraInfo)
{
	if(!(cameraInfo->flags & CAMERA_RUMBLEFADE)) return;

	// PP: update camera position offset
//temp!	updateOffsetRumble(cameraInfo->posOffset, cameraInfo->rumbleData.posOffsetVelocity, rumblePosReturnSpeed, rumblePosDamp);

	// PP: update camera lookat offset
//temp!	updateOffsetRumble(cameraInfo->lookAtOffset, cameraInfo->rumbleData.lookAtOffsetVelocity, rumbleLookAtReturnSpeed, rumbleLookAtDamp);

	// PP: update camera angle offset
	updateOffsetRumble(cameraInfo->rumbleData.angOffset, cameraInfo->rumbleData.angOffsetVelocity, rumbleAngReturnSpeed, rumbleAngDamp);
}


/* --------------------------------------------------------------------------------
   Function		: rumbleCam_explosion
   Purpose		: trigger a stock camera shake suitable for an explosion
   Parameters	: CAMERAINFO*, position of explosion, strength of explosion
   Returns		: 
   Info			: motion blur might be nice here if we're able to do it
*/
#define RC_EXPLOSION_MAX_ANG_RUMBLE			METERTOUNIT(0.007f)
#define RC_EXPLOSION_RUMBLE_RANGE			METERTOUNIT(25)
#define RC_EXPLOSION_XTILT_BIAS				0.25f

void rumbleCam_explosion(CAMERAINFO* cameraInfo, TBVector position, const float strength)
{
	if(cameraInfo != NULL)
	{
		if(cameraInfo->cameraCase != NULL)
		{
			rumbleCam_explosion(cameraInfo->cameraCase->camera, position, strength);
			return;
		}

		TBVector		rumble;				// PP: rumble vector
		TBVector		distVec;			// PP: vector distance between camera and pound point
		float			distance;			// PP: scalar distance between camera and pound point
		float			xShare, zShare;		// PP: x & z portions of rumble

		bmVectorSub(distVec, position, cameraInfo->pos);
		distance=bmVectorLen(distVec);

		// PP: get x&z shares...

		xShare=RC_EXPLOSION_XTILT_BIAS+(bmRandf()*(1.0f-RC_EXPLOSION_XTILT_BIAS));// PP: bias towards x tilt cos its sweeter
		zShare=1.0f-xShare;
		
		// PP: shake the camera angle... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		SETVECTOR(rumble, strength*xShare*((bmRand()&1)?1.0f:(-1.0f))*RC_EXPLOSION_MAX_ANG_RUMBLE,
							0.0f,
							strength*zShare*((bmRand()&1)?1.0f:(-1.0f))*RC_EXPLOSION_MAX_ANG_RUMBLE,
							0.0f);

		// PP: take into account distance falloff
		bmVectorCMul(rumble, rumble, 1.0f-MIN(1.0f, (distance/RC_EXPLOSION_RUMBLE_RANGE)));

		// PP: apply the rumble to the camera angle
		RumbleCamAng(cameraInfo, rumble);
	}
}