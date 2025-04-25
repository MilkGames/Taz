#ifndef __H_ROCKETBIKE
#define __H_ROCKETBIKE

#define MAX_ROCKETBIKE_TURN		RAD(200.0f)
#define ROCKETBIKE_SPEED		(METERTOUNIT(40.0f))
#define	ROCKETBIKE_ACCELERATION	(METERTOUNIT(5.0f))


/* --------------------------------------------------------------------------------
   Function : CreateRocketbike
   Purpose : Creates and initialises actorInstances that act as rocketbikes
   Parameters : 
   Returns : 
   Info : 
*/

void CreateRocketbike(struct TAG_ACTORINSTANCE *rocketbike);

/* --------------------------------------------------------------------------------
   Function : RespondToRocketbikeCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToRocketbikeCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *rocketbike);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToRocketbikeControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToRocketbikeControl(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnRocketbikeAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnRocketbikeAnimations(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : RocketbikeSoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void RocketbikeSoundCallback(void *context);

#endif