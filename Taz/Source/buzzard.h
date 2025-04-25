#ifndef __H_BUZZARD
#define __H_BUZZARD

#include "actors.h"

enum EBuzzardAIState
{
	// CMD: main states
	BUZZARDAI_NONE,
	BUZZARDAI_ATTACK,
	BUZZARDAI_FLY,
	BUZZARDAI_PERCH,

	// CMD: sub-states
	BUZZARDAI_END,
	BUZZARDAI_LOOP,
	BUZZARDAI_PLAYANIM,
	BUZZARDAI_TAKEOFF,
	BUZZARDAI_ASCENT,
	BUZZARDAI_LEVELOFF,
	BUZZARDAI_DECENT,
	BUZZARDAI_BEAKYFLYHOME,
	BUZZARDAI_STRAIGHT,
	BUZZARDAI_TURN,
	BUZZARDAI_BEGINAPPROACH,
	BUZZARDAI_TURNHOME,
	BUZZARDAI_LAND,
	BUZZARDAI_EGGATTACK,
};

typedef struct TAG_BUZZARD_EVENT
{
	EBuzzardAIState		action;
	char				*targetInstance;
	float				time;
} BUZZARD_EVENT, _BUZZARD_EVENT BALIGN16;

typedef int32 (*BuzzardAICallback)(ACTORINSTANCE *actorInstance, struct TAG_BUZZARDINFO *buzzardInfo);		//callback for buzzard ai
typedef int32 (*BuzzardStateChangeCallback)(ACTORINSTANCE *actorInstance, EBuzzardAIState state, EBuzzardAIState lastState);	//called when aiState changes

typedef struct TAG_BUZZARDINFO
{
	TBVector			homePos;
	TBVector			targetPos;
	TBQuaternion		homeOrientation;
	TBQuaternion		targetOrientation;

	EBuzzardAIState		aiState;						// CMD: current action being undertaken
	EBuzzardAIState		aiLastState;					// CMD: last action
	EBuzzardAIState		aiMetaState;					// CMD: general activity, eg flying, on perch
	EBuzzardAIState		aiSubState;

	BuzzardAICallback	aiCallback;
	BuzzardStateChangeCallback	stateChangeCallback;
	float				clock;
	uint32				flags;

	uint32				animCRC;
	uint32				iPad[3];

	BUZZARD_EVENT		*currentScript;
	BUZZARD_EVENT		*lastScript;
	int32				scriptIndex;
	int32				scriptIndexLastFrame;
}BUZZARDINFO, _BUZZARDINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBuzzard
   Purpose : Create buzzard or parrot
   Parameters : 
   Returns : 
   Info : 
*/

int CreateActorInstanceBuzzard(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BuzzardReturnTrueCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardReturnTrueCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info);

/* --------------------------------------------------------------------------------
   Function : BuzzardFlyHomeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardFlyHomeCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info);

/* --------------------------------------------------------------------------------
   Function : BuzzardTurnCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardTurnCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info);

/* --------------------------------------------------------------------------------
   Function : BuzzardLandCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardLandCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info);

/* --------------------------------------------------------------------------------
   Function : BuzzardEggAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardEggAttackCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info);

/* --------------------------------------------------------------------------------
   Function : BuzzardStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardAIStateChangeCallback(ACTORINSTANCE *actorInstance, EBuzzardAIState state, EBuzzardAIState lastState);

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzard
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateBuzzard(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzardLogic
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateBuzzardLogic(ACTORINSTANCE *buzzard);

#endif