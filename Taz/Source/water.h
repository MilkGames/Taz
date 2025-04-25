#ifndef __H_WATER
#define	__H_WATER

#include "collisioncache.h"

enum EWaterType
{
	WATERTYPE_WATER,
	WATERTYPE_CONCRETE,
};

typedef struct TAG_WATERRESETINFO
{
	int				numObjects;
	int				currentObject;
	char			**objectNames;
	int				pad;
} WATERRESETINFO, _WATERRESETINFO BALIGN16;

typedef struct TAG_WATERYOBJECTINFO
{
	int				type;
	uint32			numResetPoints;
	WATERRESETINFO	*resetInfo;
	float			initialCollHeight;

	int				collision;
	float			time;
	float			rippleClock;
	float			rippleCreationTime;

	BOUNDINGBOX		boundingBox;
} WATERYOBJECTINFO, _WATERYOBJECTINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateWateryObject
   Purpose : Initialises actorInstances that act as water
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateWateryObject(ACTORINSTANCE *water, struct TAG_BOUNDINGBOX *boundingBox, uint32 numResetPoints, TBVector initialPosition, EWaterType type);

/* --------------------------------------------------------------------------------
   Function : CreateWateryObjectInfo
   Purpose : Initialises actorInstanceInfo part
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateWateryObjectInfo(ACTORINSTANCE *water);

/* --------------------------------------------------------------------------------
   Function : RespondToWaterCollision
   Purpose : handles collision with water
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

int PreRespondToWaterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *water, struct TAG_PRECOLLINFO *info);

/* --------------------------------------------------------------------------------
   Function : RespondToWaterCollision
   Purpose : handles collision with water
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

int RespondToWaterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *water);

/* --------------------------------------------------------------------------------
   Function : CalculateResetPoint
   Purpose : calculates reset point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void CalculateResetPoint(ACTORINSTANCE *drowner, ACTORINSTANCE *water);

/* --------------------------------------------------------------------------------
   Function : resetActorAtSideOfWater
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfWater(ACTORINSTANCE *drowner, TBVector resetPoint);

/* --------------------------------------------------------------------------------
   Function : ResetActorAtSideOfWaterWithCartoonScaling
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfWaterWithCartoonScaling(ACTORINSTANCE *drowner, TBVector resetPoint, TBQuaternion resetQuat, EWaterType waterType);

/* --------------------------------------------------------------------------------
   Function : GetWaterResetPoints
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void GetWaterResetPoints(ACTORINSTANCE *water, uint32 numResetPoints);

/* --------------------------------------------------------------------------------
   Function : FreeWaterResetPoints
   Purpose : frees water reset points
   Parameters : ACTORINSTANCELIST *actorInstanceList
   Returns : 
   Info : 
*/

void FreeWaterResetPoints(struct TAG_WATERRESETINFO *ptr);

/* --------------------------------------------------------------------------------
   Function : CreateWaterResetInfo
   Purpose : 
   Parameters : WATERRESETINFO *
   Returns : WATERRESETINFO *
   Info : 
*/

struct TAG_WATERRESETINFO *CreateWaterResetInfo(int numObjects);

/* --------------------------------------------------------------------------------
   Function : AddWaterResetObject
   Purpose : 
   Parameters : WATERRESETINFO *, char *
   Returns : 
   Info : 
*/

int AddWaterResetObject(WATERRESETINFO *waterInfo, char*objectName);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromWater(class TAG_FADE *fade);

/* --------------------------------------------------------------------------------
   Function : ResetFadeToPoint
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void ResetFadeToPoint(class TAG_FADE *fade);

/* --------------------------------------------------------------------------------
   Function : UpdateWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void UpdateWater(ACTORINSTANCE *actorInstance);


/*  --------------------------------------------------------------------------------
	Function	: FindClosestResetPoint
	Purpose		: returns the position of the closest reset point
	Parameters	: water actor, point to test, vector to receive result, quat to receive result
	Returns		: index to reset objects, position, quaternion 
	Info		: 
*/

float FindClosestResetPoint(ACTORINSTANCE *water, TBVector point, TBVector closestPos, TBQuaternion closestQuat);

#endif