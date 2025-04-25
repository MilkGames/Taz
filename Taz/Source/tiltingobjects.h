#ifndef __H_TILTINGOBJECTS
#define __H_TILTINGOBJECTS

typedef struct TAG_TILTINGOBJECTINFO
{
	TBVector		initialPosition;
	TBQuaternion	initialOrientation;
	
	ACTORINSTANCE	*collider;
	int				type;
	int				soundHandle;
	float			maxTheta;
	float			maxPhi;
	
	float			theta;
	float			phi;
	float			maxAcceleration;
	float			maxRadius;
	
	float			timeSinceLastCollision;
	float			retardation;
	uint32			pad[2];
} TILTINGOBJECTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateTiltingObject
   Purpose : Initialises actorInstances that act as tilting objects
   Parameters : ACTORINSTANCE *, max tilt theta, max tile phi
   Returns : 
   Info : 
*/

void CreateTiltingObject(ACTORINSTANCE *actorInstance, float maxTheta, float maxPhi, float maxRadius, TBVector initPos, float maxAcceleration, float retardationForce);

/* --------------------------------------------------------------------------------
   Function : CreateTiltingObjectActorInfo
   Purpose : creates actorInfo structure
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateTiltingObjectActorInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToTiltingObjectActorInfo
   Purpose : collision callback
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int RespondToTiltingObjectCollision(ACTORINSTANCE *collider, ACTORINSTANCE *tilter);

/* --------------------------------------------------------------------------------
   Function : UpdateTiltingObject
   Purpose : updates tilting objects
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateTiltingObject(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : ProcessTilting
   Purpose : prosecces the tilt of object dependant on possition of collider
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ProcessTilting(ACTORINSTANCE *tilter);


#endif