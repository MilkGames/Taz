#ifndef __H_TAZBOSSITEMS
#define __H_TAZBOSSITEMS

#include "actors.h"
//#include "destructibles.h"


//flags
#define		BOSSITEMFLAG_DESTROYASAP	(1<<0)
#define		BOSSITEMFLAG_USECLOCK		(1<<1)
#define		BOSSITEMFLAG_RAISEMAGNET	(1<<2)
#define		BOSSITEMFLAG_LOWERMAGNET	(1<<3)
#define		BOSSITEMFLAG_CHECKTAZCRATE	(1<<4)


// CMD: constants
#define		BUTTON_GLOW_MAX_A			(40.0f)
#define		BUTTON_GLOW_SIZE			(METERTOUNIT(2.0f))

enum ETazBossItem
{
	TAZBOSSITEM_TAZCRATE,
	TAZBOSSITEM_TWEETYCRATE,
	TAZBOSSITEM_DESTRUCTIBLE,
	TAZBOSSITEM_CAMERA,
	TAZBOSSITEM_CAMERACONTROL,
	TAZBOSSITEM_DOOR,
	TAZBOSSITEM_MAGNET,
	TAZBOSSITEM_MAGNETHITPOINT,
	TAZBOSSITEM_MAGNETNODE,
	TAZBOSSITEM_DOORBUTTON,
	TAZBOSSITEM_COLLPLANE,
	TAZBOSSITEM_SHUTTER,
};

enum ETazBossItemState
{
	BOSSITEMSTATE_WAITING,
	BOSSITEMSTATE_ANIMATING,
	BOSSITEMSTATE_LOWERED,
	BOSSITEMSTATE_LOWERING,
	BOSSITEMSTATE_RAISED,
	BOSSITEMSTATE_RISING,
	BOSSITEMSTATE_DESTROYED,
	BOSSITEMSTATE_RESET,
};

typedef struct TAG_TAZBOSSITEMINFO
{
	ETazBossItem			type;
	uint32					animCRC;
	uint32					replacementCRC;
	uint32					state;

	ACTORINSTANCE			*original;
	uint32					flags;
	float					clock;
	uint32					number;
	
	uint32					destination;
	TBTexture				*texture;
	uint32					textureFrame;
	TBActorNodeInstance		*node;

	int						soundHandle;
	struct TAG_DESTRUCTIBLEDEBRISINFO	*debris;

	struct
	{
		int		r, g, b;
		float	f_alpha;
	}colour;
} TAZBOSSITEMINFO, _TAZBOSSITEMINFO;

/* --------------------------------------------------------------------------------
   Function : RandomiseButtonTimes
   Purpose : randomises the times which the buttons take to reset
   Parameters : 
   Returns : 
   Info : 
*/

void RandomiseButtonTimes(void);

/* --------------------------------------------------------------------------------
   Function : CreateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateTazBossItemInstance(ACTORINSTANCE *instance, ETazBossItem type);

/* --------------------------------------------------------------------------------
   Function : CreateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int InitialiseTazBossItemInstance(ACTORINSTANCE *instance);

/* --------------------------------------------------------------------------------
   Function : UpdateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTazBossItemInstance(ACTORINSTANCE *instance);

/* --------------------------------------------------------------------------------
   Function : DrawTazBossItemStuff
   Purpose : Draw any taz boss item stuff that doesn't get drawn elsewhere 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawTazBossItemStuff(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : RespondToTazBossBreakableCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToTazBossBreakableCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible);

/* --------------------------------------------------------------------------------
   Function : RespondToMagnetHitPointCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMagnetHitPointCollision(ACTORINSTANCE *collider, ACTORINSTANCE *hitPoint);

/* --------------------------------------------------------------------------------
   Function : RespondToDoorButtonCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToDoorButtonCollision(ACTORINSTANCE *collider, ACTORINSTANCE *piston);

/* --------------------------------------------------------------------------------
   Function : RespondToDoorButtonCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CheckForCompletionOfTazWantedGame(void);

#endif