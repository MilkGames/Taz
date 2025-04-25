#ifndef __H_QUAKECAM
#define __H_QUAKECAM


#define	CAMERA_MAX_QUAKE		(0.05f)				//max displacement of standard quake
#define CAMERA_QUAKE_TILT		(RAD(2.0f))			//max tilt of standard quake
#define	CAMERA_MAX_QUAKEBANG	(0.1f)				//max displacement of large quake
#define	QUAKE_FADE_RATE			(0.06f)

/* --------------------------------------------------------------------------------
   Function : InitCameraOffsets
   Purpose : initialises camera offsets
   Parameters : CAMERAINFO *
   Returns : 
   Info : // PP: 
*/
void InitCameraOffsets(struct TAG_CAMERAINFO* const cameraInfo);

/* --------------------------------------------------------------------------------
   Function : UpdateQuakeCam
   Purpose : controlls camera when earthquake effect is required
   Parameters : CAMERAINFO *
   Returns : 
   Info : 
*/

void UpdateQuakeCam(struct TAG_CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : CheckQuakeForOpposites
   Purpose : check where offset was last frame, and correct offset so that new offset is on other side of origin
   Parameters : TBVector *2
   Returns : 
   Info : 
*/

void CheckQuakeVectorForOpposites(TBVector old, TBVector current);

/* --------------------------------------------------------------------------------
   Function : CheckQuakeTiltForOpposites
   Purpose : same as above, but for a float
   Parameters : TBVector *2
   Returns : 
   Info : 
*/

float CheckQuakeTiltForOpposites(float old, float current);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												'RUMBLE' STUFF
											for want of a better name
  
											Smooth camera shake effects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern float rumblePosReturnSpeed;			// PP: TODO: comment
extern float rumblePosDamp;					// PP: TODO: comment

extern float rumbleLookAtReturnSpeed;		// PP: TODO: comment
extern float rumbleLookAtDamp;				// PP: TODO: comment

extern float rumbleAngReturnSpeed;			// PP: TODO: comment
extern float rumbleAngDamp;					// PP: TODO: comment


/* --------------------------------------------------------------------------------
   Function : UpdateRumbleCam
   Purpose : updates rumble effects on a camera
   Parameters : CAMERAINFO *
   Returns : 
   Info : 
*/
void UpdateRumbleCam(struct TAG_CAMERAINFO *cameraInfo);


/* --------------------------------------------------------------------------------
   Function : RumbleCamLookat
   Purpose : rumble a camera's lookat point
   Parameters : CAMERAINFO *, rumble vector
   Returns : 
   Info : 
*/
inline void RumbleCamLookat(struct TAG_CAMERAINFO *const cameraInfo, TBVector rumble)
{
	cameraInfo->flags |= CAMERA_RUMBLEFADE;
	bmVectorAdd(cameraInfo->rumbleData.lookAtOffsetVelocity, cameraInfo->rumbleData.lookAtOffsetVelocity, rumble);
}


/* --------------------------------------------------------------------------------
   Function : RumbleCamPos
   Purpose : rumble a camera's position
   Parameters : CAMERAINFO *, rumble vector
   Returns : 
   Info : 
*/
inline void RumbleCamPos(struct TAG_CAMERAINFO *const cameraInfo, TBVector rumble)
{
	cameraInfo->flags |= CAMERA_RUMBLEFADE;
	bmVectorAdd(cameraInfo->rumbleData.posOffsetVelocity, cameraInfo->rumbleData.posOffsetVelocity, rumble);

	// CMD: clamp velocity
	if(bmVectorLen(cameraInfo->rumbleData.posOffsetVelocity) > RAD(120.0f))
	{
		bmVectorScaleToLength(cameraInfo->rumbleData.posOffsetVelocity, cameraInfo->rumbleData.posOffsetVelocity, RAD(120.0f));
	}
}


/* --------------------------------------------------------------------------------
   Function : RumbleCamAng
   Purpose : rumble a camera's angle
   Parameters : CAMERAINFO *, rumble vector
   Returns : 
   Info : 
*/
inline void RumbleCamAng(struct TAG_CAMERAINFO *const cameraInfo, TBVector rumble)
{
	cameraInfo->flags |= CAMERA_RUMBLEFADE;
	bmVectorAdd(cameraInfo->rumbleData.angOffsetVelocity, cameraInfo->rumbleData.angOffsetVelocity, rumble);

	// CMD: clamp velocity
	if(bmVectorLen(cameraInfo->rumbleData.angOffsetVelocity) > RAD(120.0f))
	{
		bmVectorScaleToLength(cameraInfo->rumbleData.angOffsetVelocity, cameraInfo->rumbleData.angOffsetVelocity, RAD(120.0f));
	}
}


/* --------------------------------------------------------------------------------
   Function		: rumbleCam_explosion
   Purpose		: trigger a stock camera shake suitable for an explosion
   Parameters	: CAMERAINFO*, position of explosion, strength of explosion
   Returns		: 
   Info			: motion blur might be nice here if we're able to do it
*/
void rumbleCam_explosion(struct TAG_CAMERAINFO* cameraInfo, TBVector position, const float strength);


#endif