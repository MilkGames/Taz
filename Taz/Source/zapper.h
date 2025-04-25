#ifndef __H_ZAPPER
#define __H_ZAPPER

#include "actors.h"
#include "lightning.h"

#define ZAPPER_EFFECT_TIME	(1.0f)

// CMD: flags
#define ZAPPER_ACTIVE		(1<<0)

enum EZapperType
{
	ZAPPER_MOUSE,
	ZAPPER_BALL,
	ZAPPER_CRATE,
	ZAPPER_TAZ,
	ZAPPER_NONE,
};

typedef struct TAG_ZAPPERINFO
{
	EZapperType		type;
	uint32			flags;
	float			effectClock;
	int32			iPad;

	LIGHTNINGINFO	lightningInfo;
}ZAPPERINFO, _ZAPPERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateZapper
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateZapper(ACTORINSTANCE *actorInstance, EZapperType type);

void BallStateChangeCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateZapper
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateZapper(ACTORINSTANCE *actorInstance);

#endif