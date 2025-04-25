// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
// PP: Adapted from Fred's Party-Game Rowing camera, which was a re-write of 
// PP:	my own Rowing camera anyway, so I feel less guilty somehow!
// PP:	It is the Great Circle of Life.
//
//      File : KOScam.cpp
//   Purpose : "Keep-On-Screen" camera - keeps its target actor instances on-screen at all times
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"								// PP: global stuff
#include "display.h"							// PP: display stuff
#include "main.h"								// PP: main stuff
#include "maths.h"								// PP: maths stuff
#include "actors.h"								// PP: actor stuff
#include "camera.h"								// PP: camera stuff
#include "cameracase.h"							// PP: camera case stuff
#include "KOScam.h"								// PP: "Keep-On-Screen" camera

#define KOSCAM_MIN_DISTANCE						2000.0f


/* --------------------------------------------------------------------------------
   Function : KOScam_SelectVisiblePoints
   Purpose : Translate the player positions into camera-relative orientation. (not position)
				Fill in 4 player positions, moved out a bit from their centre, and the centre point
   Parameters : 
   Returns : 
   Info : 
*/
static void KOScam_SelectVisiblePoints(KOSCAMDATA* const camData, TBVector *vectors)
{
	int				i=0;
	TBVector		mid;
	KOSCAMTARGET*	target;

	// Grab the positions of the things we want to keep on screen
	for(i=camData->numTargets()-1; i>=0; i--)
	{
		target=camData->getTarget(i);
		bmVectorCopy(vectors[i], target->actorInstance->position);

		// PP: apply y offset of centre point
		vectors[i][Y] += target->yOffset;
	}

	mid[X] = mid[Y] = mid[Z] = 0.0f;
	mid[W] = 1.0f;

	for(i=camData->numTargets()-1; i>=0; i--)
	{
		bmVectorAdd(mid, mid,vectors[i]);
	}


	bmVectorCDiv(mid, mid, float(camData->numTargets()));


	// Now we need to push the "player points" away from the centre
	for(i=camData->numTargets()-1; i>=0; i--)
	{
		target=camData->getTarget(i);

		TBVector temp,temp2;
		bmVectorSub(temp,vectors[i],mid);
		if(temp[X] || temp[Z])
		{
			temp[W] = 1.0f;
			bmVectorScaleToLength(temp2,temp,target->radius);
			bmVectorAdd(vectors[i],vectors[i],temp2);
		}
	}
}


#define YANG1(_ang)		((_ang))
#define YANG2(_ang)		(-((_ang)))


/* --------------------------------------------------------------------------------
   Function : KOScam_update
   Purpose : update the camera
   Parameters : ptr to camera case
   Returns : 
   Info : 
*/
void KOScam_update(CAMERACASE* const camCase)
{
	static float				min_range=2000.0f;							// PP: hmm not too sure what this does yet
	float						use_yang=camCase->KOScamData->angle[Y];		// PP: hmm not too sure what this does yet
	TBVector					camPos;										// PP: vector to which to set the camera's position
	TBVector					camLookat;									// PP: vector to which to set the camera's lookat point

	TBMatrix matrix;
	TBVector srcpoints[10];
	TBVector workpoints[10];
	TBVector outpoint;
	TBVector outtarget;

	TBVector average;

	int i;
	float xm, ym;

	float max_c, min_c;


	// Decide what points we want to be able to see...
	KOScam_SelectVisiblePoints(camCase->KOScamData, srcpoints);


	// Stick the points into camera-rotation space, and grab an average value
	bmMatZXYRotation(matrix,camCase->KOScamData->angle[X],YANG1(camCase->KOScamData->angle[Y]),0.0f);	// This is z-first, then X, then Y (ie, an anti-rotation. World-to-camera)

	average[X]=average[Y]=average[Z]=0.0f; average[W]=1.0f;

	for (i=camCase->KOScamData->numTargets()-1;i>=0;i--)
	{
		srcpoints[i][W]=1.0f;
		bmMatMultiplyVector2(workpoints[i], matrix, srcpoints[i]);
		bmVectorAdd(average,average,workpoints[i]);
	}

	bmVectorCDiv(average, average, float(camCase->KOScamData->numTargets()));


	xm = bmSin(CURRENT_SCREEN.fov/2.0f);
	ym = xm * 0.75f;

// Establish the top & bottom extremes, and the Y/Z coord (in camera-orientation)
// that the camera needs to be at in order to see them all

//   okeydokey, now we want the point with the min & max "c" value for (y=mx+c),
//   where we know y,x (from our point),and m (from the FOV)
//   c = y-mx (Where X, in this case, is actually the Z coord, and Y is either X or Y depending which screen axis we're tesing

	float far_z;


	far_z = workpoints[0][Z];
	max_c = workpoints[0][Y] - ym * workpoints[0][Z];	// +ve gradient
	min_c = workpoints[0][Y] + ym * workpoints[0][Z];	// -ve gradient
	
	for(i = camCase->KOScamData->numTargets()-1; i>=0; i--)
	{
		float c;
		c = workpoints[i][Y] - ym * workpoints[i][Z];	// +ve gradient

		if(workpoints[i][Z] > far_z)
			far_z = workpoints[i][Z];

		if(c > max_c)
		{
			max_c = c;
		}

		c = workpoints[i][Y] + ym * workpoints[i][Z];	// -ve gradient
		if(c < min_c)
		{
			min_c = c;
		}
	}

	// Y is midway between the two "c" axis-intercepts, regardless of where the axis origin is...
	outpoint[Y] = (min_c + max_c) / 2.0f;

	// Z's half of the axis-intercept range, projected out by the gradient
	outpoint[Z] = -((max_c - min_c)/2.0f) / ym;



// Establish the left & right extremes, and the X/Z coord (in camera-orientation)
// that the camera needs to be at in order to see them all. Same mechanism. wider gradient.

	max_c = workpoints[0][X] - xm * workpoints[0][Z];
	min_c = workpoints[0][X] + xm * workpoints[0][Z];
	for(i = camCase->KOScamData->numTargets()-1; i>=0; i--)
	{
		float c;
		c = workpoints[i][X] - xm * workpoints[i][Z];

		if(c > max_c)
		{
			max_c = c;
		}

		c = workpoints[i][X] + xm * workpoints[i][Z];
		if(c < min_c)
		{
			min_c = c;
		}
	}

	outpoint[X] = (min_c + max_c) / 2.0f;

// distance is whichever of the two was higher... Um, or lower. Camera is -ve Z in these conditions, I think...
	float tz = - ((max_c - min_c) / 2.0f) / xm;
	if(tz < outpoint[Z]) outpoint[Z] = tz;

// Set up a target point (again, in camera-oriented space)
// Enforce a minimum range

	bmVectorCopy(outtarget,outpoint);
	outtarget[Z] = average[Z];

/*	if(outtarget[Z] - outpoint[Z] < min_range)
		outpoint[Z] = outtarget[Z] - min_range;*/

// Rotate the source & target points back into worldspace

	bmMatYXZRotation(matrix,-camCase->KOScamData->angle[X],YANG2(camCase->KOScamData->angle[Y]),0.0f);
	outpoint[W] = 1.0f;
	outtarget[W] = 1.0f;

	bmMatMultiplyVector2(camPos, matrix, outpoint);
	bmMatMultiplyVector2(camLookat, matrix, outtarget);

	SetCameraSmooth(camCase->camera, defaultPosSmooth, defaultLookatSmooth);

	SetCameraPositionDestination(camCase->camera, camPos);
	SetCameraLookAtDestination(camCase->camera, camLookat);
}
