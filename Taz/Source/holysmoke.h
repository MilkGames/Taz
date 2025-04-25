#ifndef __H_HOLYSMOKE
#define __H_HOLYSMOKE

#include "actors.h"
#include "debris.h"

#define	MAX_SMOKE_SPRITES		(1)
#define MAX_BIG_FALL_SPRITES	(30)
#define SLOW_SMOKE_SPEED		(METERTOUNIT(0.12f))
#define FAST_SMOKE_SPEED		(METERTOUNIT(1.5f))

enum
{
	SMOKE_SKIDSTOP,
	SMOKE_SPIN,
	SMOKE_TAZSPIN,
	SMOKE_SMALLFALL,
	SMOKE_BIGFALL,
	SMOKE_EXPLOSION,
	SMOKE_COWBOYFINGER,
};

extern float cloudCounter;

/*	--------------------------------------------------------------------------------
	Function 	: CreateImpactClouds
	Purpose 	: creates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int CreateImpactClouds(TBVector position, TBVector windVelocity, TBVector velocity, float maxScale, float life, float maxSpeed, uint32 smokeType);

/*	--------------------------------------------------------------------------------
	Function 	: InitSkidSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitSkidSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: InitSpinSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitTazSpinSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: InitSpinSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitSpinSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: InitFallSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitFallSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: InitExplosionSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitExplosionSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: InitFingerSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitFingerSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateImpactClouds
	Purpose 	: updates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateImpactClouds(SMOKEEXTRAINFO *data);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateExplosionSmoke
	Purpose 	: updates clouds that appear when an explosion has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateExplosionSmoke(SMOKEEXTRAINFO *data);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFingerSmoke
	Purpose 	: updates clouds that appear when taz does his cowboy shoot attack
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateFingerSmoke(SMOKEEXTRAINFO *data);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateImpactClouds
	Purpose 	: updates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawSmokeAtNodeInstance(ACTORINSTANCE *actorInstance, char *nodeName, uint32 nodeNameCRC, uint32 smokeType, float maxScale=25.0f);

#endif