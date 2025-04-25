#ifndef __H_MAGNET
#define __H_MAGNET

#include "actors.h"

#define MAGNET_SWITCHRATE		(2.0f)
#define MAGNET_NUM_PLAYERS		(2)
#define MAGNET_NUM_STREAKS		(5)								// CMD: number of streaks per player per magnet
#define MAGNET_NUM_POINTS		(10)
#define MAGNET_START_PERIOD		(2.0f)
#define MAGNET_JITTER_PERIOD	(1.0f)
#define MAGNET_MAX_VEL			(METERTOUNIT(3.0f))

enum EMagnetState
{
	MAGNETSTATE_NEUTRAL,
	MAGNETSTATE_ATTRACTIVE,
	MAGNETSTATE_REPULSIVE,
};

typedef struct TAG_MAGNETSTREAKINFO
{
	TBVector		startPoint;
	TBVector		ctrlPts[2];
	TBVector		endJitter;
	
	TBVector		points[MAGNET_NUM_POINTS];
	float			size[MAGNET_NUM_POINTS];
	int				colour[MAGNET_NUM_POINTS][4];

	float			ctrlClock;
	float			jitterClock;
	float			startClock;
} MAGNETSTREAKINFO, _MAGNETSTREAKINFO BALIGN16;

typedef struct TAG_MAGNETPLAYERINFO
{
	struct
	{
		MAGNETSTREAKINFO	streakInfo[MAGNET_NUM_STREAKS];
		TBVector	initCtrlPts[2];
	} effectInfo;

	float			minRadius;
	float			maxRadius;
	int				magSoundChannel;

	ACTORINSTANCE	*actorEffected;
	EMagnetState	state;
} MAGNETPLAYERINFO, _MAGNETPLAYERINFO BALIGN16;

typedef struct TAG_MAGNETINFO
{
	MAGNETPLAYERINFO	character[MAGNET_NUM_PLAYERS];
} MAGNETINFO, _MAGNETINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateMagnetInstance
   Purpose : makes instances act like magnets
   Parameters : fairly obvious stuff!
   Returns : true/false dependant on success
   Info : If the traditionally shaped magnet were to be used as a horseshoe, horses would be a great deal more versatile
*/

MAGNETINFO *CreateMagnetInstance(ACTORINSTANCE *actorInstance, float minRadius, float maxRadius);

/* --------------------------------------------------------------------------------
   Function : UpdateMagnetInstance
   Purpose : updates magnet instances
   Parameters : magnet instance actorInstance
   Returns : 
   Info : willy weasil is, contrary to general opinion, a fantastic game character
*/

void UpdateMagnetInstance(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawMagnetStuff
   Purpose : draws magnet stuff, of course
   Parameters : magnet instance actorInstance
   Returns : 
   Info : 
*/

void DrawMagnetStuff(ACTORINSTANCELIST *actorInstanceList);

#endif