#ifndef __H_COSTUMES
#define __H_COSTUMES

#include "scene.h"
#include "voodoospelleffect.h"

#define LOSECOSTUME_GLOWBALLTIME	(0.75f)
#define LOSECOSTUME_GLOWBALLSIZE	(METERTOUNIT(2.5f))
#define LOSECOSTUME_WHITEOUTSIZE	(METERTOUNIT(10.0f))
#define LOSECOSTUME_SPITTIME		(0.5f)
#define LOSECOSTUME_TOTALTIME		(3.0f)

enum ELoseCostumeChange
{
	LOSECOSTUME_GLOW,
	LOSECOSTUME_BEAMS,
	LOSECOSTUME_WHITEOUT,
};

enum ECostume
{
	COSTUME_NONE			= -1,	// PP: no costume, the default parameter for SetupCostume.  Sometimes we don't want to setup a specific costume, just a costume accessory specified by a cheat, eg. the DIGITALWATCH cheat.

	COSTUME_NINJA,
	COSTUME_COWBOY,
	COSTUME_CONSTRUCTION,			// TP: No special move
	COSTUME_REINDEER,				// TP: No special move
	COSTUME_EXPLORER,				// TP: No special move
	COSTUME_SURFER,
	COSTUME_RAPPA,
	COSTUME_WERETAZ,
	COSTUME_MINER,					// TP: No special move
	COSTUME_INDY,
	COSTUME_TARZAN,
	COSTUME_SNOWBOARD,
	COSTUME_SWAT,
	COSTUME_SKATER,
	COSTUME_TRIPPY,
};

typedef struct TAG_LOSECOSTUMEINFO
{
	TBVector			position;

	float				size;
	float				sizeRateOfChange;			// CMD: in meters per second
	float				clock;
	VOODOOSPELLFXINFO	*spellEffectInfo;

	ELoseCostumeChange	state;
	float				alpha;
	TBActorNodeInstance	*node;
	int32				iPad;
}LOSECOSTUMEINFO, _LOSECOSTUMEINFO BALIGN16;

/*	--------------------------------------------------------------------------------
	Function 	: SetupLevelCostume
	Purpose 	: Dress an ACTORINSTANCE in the appropriate attire for the current level
	Parameters 	: ACTORINSTANCE to dress
	Returns 	: 
	Info 		: // PP: 
*/
void SetupLevelCostume(ACTORINSTANCE* const actorInstance);

/* --------------------------------------------------------------------------------
   Function : SetupCostume
   Purpose : Changes Taz into costume Taz by attaching new objects
   Parameters : ACTORINSTANCE, (opt/none)costume
   Returns : 
   Info : Must be called inside phone box and after exiting spin.
*/
int SetupCostume(ACTORINSTANCE *actorInstance,char costume=COSTUME_NONE);

/* --------------------------------------------------------------------------------
   Function : RemoveCostume
   Purpose : Remove any costume Taz is currently wearing
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int RemoveCostume(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: DoTazanCostumeMove
	Purpose		: Called when the player summons the elephants
	Parameters	: ACTORINSTANCE for player
	Returns		: 
	Info		: 
*/

void DoTazanCostumeMove(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

LOSECOSTUMEINFO *CreateLoseCostumeEffect(ACTORINSTANCE *actorInstance, char *nodeName, float duration);

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

LOSECOSTUMEINFO *UpdateLoseCostumeEffect(ACTORINSTANCE *actorInstance, LOSECOSTUMEINFO *info);

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

void DrawLoseCostumeEffect(ACTORINSTANCELIST *actorInstanceList);

/*  --------------------------------------------------------------------------------
	Function	: GetNaked
	Purpose		: For when Taz wants to undress
	Parameters	: 
	Returns		: 
	Info		: 
*/

int GetNaked(ACTORINSTANCE *actorInstance, EScene scene);

/*  --------------------------------------------------------------------------------
	Function	: GetNaked
	Purpose		: For when Taz wants to undress
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitialiseNakedness(EScene scene);

#endif