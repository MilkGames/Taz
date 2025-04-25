#ifndef __H_TROLLEY
#define __H_TROLLEY

#define	TROLLEY_ACCELERATION	(METERTOUNIT(2.0f))
#define TROLLEY_DECELERATION	(METERTOUNIT(4.0f))

/* --------------------------------------------------------------------------------
   Function : CreatePneumaticPlunger
   Purpose : Creates and initialises actorInstances that act as pneumaticplungers
   Parameters : 
   Returns : 
   Info : 
*/

void CreateTrolley(struct TAG_ACTORINSTANCE *trolley);

/* --------------------------------------------------------------------------------
   Function : RespondToTrolleyCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToTrolleyCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *trolley);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToTrolleyControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToTrolleyControl(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTrolleyAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnTrolleyAnimations(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : TrolleySoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void TrolleySoundCallback(void *context);

#endif