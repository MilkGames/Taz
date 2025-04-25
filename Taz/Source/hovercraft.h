#ifndef __H_HOVERCRAFT
#define __H_HOVERCRAFT

#define HOVERCRAFT_MAX_ROT_ACCEL	(RAD(180.0f))
#define	HOVERCRAFT_MAX_ROT_INPUT	(RAD(90.0f))
#define	HOVERCRAFT_MAX_VELOCITY		(METERTOUNIT(20.0f))
#define	HOVERCRAFT_THRUST			(METERTOUNIT(10.0f))
#define HOVERCRAFT_ROT_FRIC			(RAD(30.0f))

/* --------------------------------------------------------------------------------
   Function : CreateHovercraft
   Purpose : Creates and initialises actorInstances that act as hovercrafts
   Parameters : 
   Returns : 
   Info : 
*/

void CreateHovercraft(struct TAG_ACTORINSTANCE *hovercraft);

/* --------------------------------------------------------------------------------
   Function : RespondToHovercraftCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToHovercraftCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *hovercraft);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToHovercraftControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToHovercraftControl(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnHovercraftAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnHovercraftAnimations(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_PADCONTROLSTATUS *controller);

#endif