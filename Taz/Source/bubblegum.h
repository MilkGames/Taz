#ifndef __H_BUBBLEGUM
#define __H_BUBBLEGUM

#define BUBBLEGUM_TIME		(25.0f)
#define BUBBLEGUM_ROTATING_VECTOR_LENGTH	(METERTOUNIT(3.0f))
#define BUBBLEGUM_SCALE_CONST	(0.05f)

enum EBubbleGumStates
{
	BUBBLEGUM_RISING,
	BUBBLEGUM_STABLE,
};

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBubbleGumPowerUpForCharacter(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AttachGumBubbleToActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int	AttachGumBubbleToActor(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateGumBubble
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateGumBubble(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: FinishBubbleGum
	Purpose		: Sort out Taz after bubble gum move
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: Made this seperate function so it can be called if the player dies
*/
void FinishBubbleGum(struct TAG_ACTORINSTANCE *actorInstance);

#endif