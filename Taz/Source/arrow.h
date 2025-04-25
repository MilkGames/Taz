#ifndef __H_ARROW
#define __H_ARROW

#include "actors.h"

#define SAFARIARROW_MAXROT		(RAD(360.0f))

enum EArrowState
{
	ARROW_NORMAL,
	ARROW_FADE,
	ARROW_DEAD,
};

typedef struct TAG_SAFARIARROWINFO
{
	TBQuaternion	initOrientation;

	float			rotAngle;
	float			rotSpeed;
	float			rotAcceleration;
	float			clock;

	float			phaseAngle;
	float			spinClock;
	int32			spin;
	EArrowState		state;

	short			alpha;
	short			sPad;
	int32			iPad[3];
}SAFARIARROWINFO, _SAFARIARROWINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateSafariArrow(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSafariArrow(ACTORINSTANCE *actorInstance);

#endif