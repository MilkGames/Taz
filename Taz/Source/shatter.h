#ifndef __H_SHATTER
#define __H_SHATTER

enum
{
	SHATTERTYPE_DEFAULT,
	SHATTERTYPE_SPIN,
	SHATTERTYPE_TRIGGER,
	SHATTERTYPE_MARIOPAINTING,
	SHATTERTYPE_MARIOWAITING,
};

enum 
{
	SHATTERSTATE_IDLE,
	SHATTERSTATE_DESTROYANIM,
	SHATTERSTATE_SHATTER
};

typedef struct TAG_SHATTERINFO
{
	uint32				shatterType;
	uint32				triggerType;
	int					destroy;
	int					collision;

	float				delay;
	float				clock;
	struct TAG_TRIGGERINFO	*onShatter;
	uint32				idleAnimCRC;

	uint32				destroyAnimCRC;
	uint32				state;
	uint32				textureCRC;
	int32				iPad;
} SHATTERINFO, _SHATTERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateShatterObject
   Purpose : creates an object which waits to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

int CreateShatterObject(struct TAG_ACTORINSTANCE *actorInstance, uint32 shatterType, int destroy, uint32 triggerType, float delay,uint32 idleAnimCRC,uint32 destroyAnimCRC, uint32 textureCRC=0);

/* --------------------------------------------------------------------------------
   Function : CreateShatterObject
   Purpose : creates an object which waits to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

SHATTERINFO *CreateShatterInfo(void);

/* --------------------------------------------------------------------------------
   Function : UpdateShatterObject
   Purpose : updates an object which is waiting to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateShatterObject(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : RespondToShattnerCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int PreRespondToShatterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *shatter, struct TAG_PRECOLLINFO *info);

/* --------------------------------------------------------------------------------
   Function : RespondToShattnerCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToShatterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *shatter);

#endif