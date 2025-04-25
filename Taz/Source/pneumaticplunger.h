#ifndef __H_PNEUMATICPLUNGER
#define __H_PNEUMATICPLUNGER

typedef struct TAG_PNEUMATICPLUNGERINFO
{
	TBVector			initialPosition;

	uint32				onPlunger;
	uint32				pad[3];
} PNEUMATICPLUNGERINFO BALIGN16;

typedef struct TAG_CURRENTPLUNGERINFO
{
	TBVector			homePosition;

	int					handle;
	int					pad[3];
} CURRENTPLUNGERINFO BALIGN16;					//info relating to plunger that taz is on

extern CURRENTPLUNGERINFO		currentPlungerInfo;

/* --------------------------------------------------------------------------------
   Function : CreatePneumaticPlunger
   Purpose : Creates and initialises actorInstances that act as pneumaticplungers
   Parameters : 
   Returns : 
   Info : 
*/

void CreatePneumaticPlunger(TBVector position);

/* --------------------------------------------------------------------------------
   Function : CreatePneumeticPlungerInfo
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int CreatePneumaticPlungerInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdatePneumeticPlunger
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void UpdatePneumaticPlunger(ACTORINSTANCE *plunger);

/* --------------------------------------------------------------------------------
   Function : RespondToPneumaticPlungerCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToPneumaticPlungerCollision(ACTORINSTANCE *collider, ACTORINSTANCE *plunger);

#endif