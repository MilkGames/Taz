#ifndef __H_FOUNTAIN
#define __H_FOUNTAIN

#define	FOUNTAIN_VELOCITY	(METERTOUNIT(8.0f))
#define FOUNTAIN_STALLTIME	(1.0f)

enum
{
	FOUNTAIN_MINIMUM,
	FOUNTAIN_RISE,
	FOUNTAIN_MAXIMUM,
	FOUNTAIN_FALL,
	FOUNTAIN_STALL,
};

typedef struct TAG_FOUNTAININFO
{
	TBVector	initialPosition;

	TBVector	velocity;

	float		maxHeight;
	float		period;
	float		clock;
	ACTORINSTANCE	*characterOnFountain;
	
	int			fountainState;
	int			idle;
	int			active;
	int			scenic;
	} FOUNTAININFO, _FOUNTAININFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateFountain
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateFountain(ACTORINSTANCE *actorInstance, float maxHeight, float period, int idle);

/* --------------------------------------------------------------------------------
   Function : CreateFountainInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

FOUNTAININFO *CreateFountainInfo(void);

/* --------------------------------------------------------------------------------
   Function : RespondToFountainCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToFountainCollision(ACTORINSTANCE *collider, ACTORINSTANCE *fountain);

/* --------------------------------------------------------------------------------
   Function : ProcessFountainPhysics
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ProcessFountainPhysics(ACTORINSTANCE *fountain);

/* --------------------------------------------------------------------------------
   Function : UpdateFountains
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFountains(ACTORINSTANCE *ptr);

#endif