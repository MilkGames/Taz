#ifndef __H_CHILLIPEPPER
#define __H_CHILLIPEPPER

#define CHILLIPEPPER_SMOKE_SIZE		(METERTOUNIT(0.04f))
#define CHILLIPEPPER_SMOKE_LIFE		(1.0f)
#define CHILLIPEPPER_SMOKE_SPEED	(METERTOUNIT(0.9f))
#define CHILLIPEPPER_SMOKE_MIN_SPEED	(METERTOUNIT(0.25f))

#define CHILLIPEPPER_MAX_ALPHA		(127)				// SO: Initial alpha value
#define CHILLIPEPPER_FADE_LIFE		(0.15f)				// SO: Life left before fade

#define MAX_CHILLI_FIRE				(500)
#define CHILLIPEPPER_FIRE_SPEED		(METERTOUNIT(12.0f))
#define	CHILLIPEPPER_FIRE_LIFE		(1.0f)
#define CHILLIPEPPER_FIRE_MINSIZE	(METERTOUNIT(0.1f))
#define CHILLIPEPPER_FIRE_MAXSIZE	(METERTOUNIT(0.15f))
#define CHILLIPEPPER_FIRE_RANDDISP	(100)
#define CHILLIPEPPER_FIRE_DISP		(30)
#define CHILLIPEPPER_FIRE_NUM		(2)
#define CHILLIPEPPER_FIRE_RANGE		(METERTOUNIT(2.0f))

#define CHILLIPEPPER_EASY			(30.0f)
#define CHILLIPEPPER_MEDIUM			(25.0f)
#define CHILLIPEPPER_HARD			(20.0f)

#define	MEGA_CHILLIPEPPER_FIRE_LIFE		(3.5f)				// SO: Mega chilli's have more life

typedef struct TAG_CHILLIPEPPEREXTRAS
{
	TBVector		position;
	TBVector		velocity;

	float			size;
	float			rotation;
	TBTexture		*texture;
	float			life;

	float			colour[4];
	
	int32			playerNo;
	uint32			flags;
	int32			iPad[2];
} CHILLIPEPPEREXTRAS, _CHILLIPEPPEREXTRAS BALIGN16;

extern TBExtraSystem	*chilliFireSystem;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateCharacterChilliPepperSmoke(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ChilliPepperExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharacterChilliPepperPowerUp(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ChilliPepperExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void ChilliPepperExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraRenderCallback
   Purpose : This function draws the chilli pepper powerup fire
   Parameters : 
   Returns : 
   Info : 
*/

void ChilliPepperExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateChilliPepperFlameThrower(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperFire(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperFireCollision(CHILLIPEPPEREXTRAS *data);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperTazCollision(CHILLIPEPPEREXTRAS *data);

/*  --------------------------------------------------------------------------------
	Function	: FinishChilliPepper
	Purpose		: Reset Taz after a chilli pepper encounter
	Parameters	: actorinstance
	Returns		: 
	Info		: 
*/
void FinishChilliPepper(struct TAG_ACTORINSTANCE *actorInstance);

#endif
