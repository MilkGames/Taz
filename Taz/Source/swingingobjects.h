#ifndef __H_SWINGINGOBJECTS
#define __H_SWINGINGOBJECTS

#include "actors.h"

enum
{
	SWINGMODEL_MONKEY,
	SWINGMODEL_CHANDELIER,
	SWINGMODEL_CHANDTRIGGER,
	SWINGMODEL_CRATEONROPE,
};
typedef struct TAG_SWINGINGOBJECTINFO
{
	TBVector			initialPosition;

	int					type;
	float				length;
	float				theta;
	float				phi;

	uint32				collision;
	int32				model;							//model associated with rope
	ACTORINSTANCE		*modelPtr;						//ptr to model's actorinstance structure
	struct TAG_TRIGGERINFO		*onContact;

	int					soundHandle;
	uint32				modelCRC;
	int					iPad[2];
} SWINGINGOBJECTINFO , _SWINGINGOBJECTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateSwingingObject
   Purpose : Initialises actorInstances that act as swingers
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSwingingObject(ACTORINSTANCE *swinger, TBVector initialPosition, float length, int32 model, char *obe);

/* --------------------------------------------------------------------------------
   Function : FindSwingingModelsFromMap
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void FindSwingingModelsFromMap(ACTORINSTANCELIST *instanceList);

/* --------------------------------------------------------------------------------
   Function : CreateSwingingObjectActorInfo
   Purpose : Initialises actorInstances info structure
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSwingingObjectActorInfo(ACTORINSTANCE *swinger);

/* --------------------------------------------------------------------------------
   Function : RespondToSwingerCollision
   Purpose : collision callback
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

int RespondToSwingerCollision(ACTORINSTANCE *collider, ACTORINSTANCE *swinger);

/* --------------------------------------------------------------------------------
   Function : UpdateSwingingObject
   Purpose : updates swinging object
   Parameters : ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

void UpdateSwingingObject(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : ProcessSwing
   Purpose : calculates swing stuff
   Parameters : ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

void ProcessSwing(ACTORINSTANCE *swinger);

/* --------------------------------------------------------------------------------
   Function : SwingTaz
   Purpose : calculates swing when taz is swinging
   Parameters : ACTORINSTANCE *tazInstance
   Returns : 
   Info : 
*/

void SwingActor(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : CalculateOrientationOfRope
   Purpose : name says it all
   Parameters : ACTORINSTANCE *actor, ACTORINSTANCE *rope
   Returns : 
   Info : 
*/

void CalculateOrientationOfRope(TBVector actorPosition, ACTORINSTANCE *rope, ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : CalculateRotationAboutYAxis
   Purpose : calculates actors rotation about y axis, before collision with rope
   Parameters : ACTORINSTANCE *actor
   Returns : 
   Info : 
*/

void CalculateRotationAboutYAxis(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForMonkey
   Purpose : see above
   Parameters : ACTORINSTANCE *actorInstance
   Returns : TBActorAnimSegment *animSegment
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForMonkey(ACTORINSTANCE *monkey);

#endif