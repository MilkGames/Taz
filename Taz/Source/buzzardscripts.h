#ifndef __H_BUZZARDSCRIPTS
#define __H_BUZZARDSCRIPTS

#include "actors.h"
#include "maths.h"
#include "buzzard.h"

// CMD: main states
//		BUZZARDAI_NONE,
//		BUZZARDAI_ATTACK,
//		BUZZARDAI_LAND,
//		BUZZARDAI_FLY,
//		BUZZARDAI_PERCH,

// CMD: sub-states
//		BUZZARDAI_PLAYANIM,
//		BUZZARDAI_END
//		BUZZARDAI_TAKEOFF,

static BUZZARD_EVENT	beakyIdle1[] = 
{
	{BUZZARDAI_PLAYANIM, "idle1", 3.0f},
	{BUZZARDAI_PLAYANIM, "idle2", 1.5f},
	{BUZZARDAI_PLAYANIM, "idle6", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle3", 1.5f},
	{BUZZARDAI_PLAYANIM, "idle6", 0.0f},
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	beakyIdle2[] = 
{
	{BUZZARDAI_PLAYANIM, "idle4", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle5", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle7", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle8", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle10", 0.0f},
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	beakyIdle3[] = 
{
	{BUZZARDAI_PLAYANIM, "idle1", 2.0f},
	{BUZZARDAI_PLAYANIM, "idle10", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle6", 0.0f},
	{BUZZARDAI_PLAYANIM, "idle9", 0.0f},
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	beakyTakeOff[] = 
{
	{BUZZARDAI_PLAYANIM, "idle10", 0.0f},
	{BUZZARDAI_TAKEOFF, NULL, METERTOUNIT(3.0f)},
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	standardFly[] = 
{
	{BUZZARDAI_PLAYANIM, "fly1", 4.0f},
	{BUZZARDAI_BEAKYFLYHOME, "fly2", 45.0f},			// CMD: turn at x rads per second
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	breaststrokeFly[] = 
{
	{BUZZARDAI_PLAYANIM, "fly4", 4.0f},
	{BUZZARDAI_BEAKYFLYHOME, "fly2", 45.0f},			// CMD: turn at x rads per second
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	frontcrawlFly[] = 
{
	{BUZZARDAI_PLAYANIM, "tween1", 0.0f},				// CMD: into crawl
	{BUZZARDAI_PLAYANIM, "fly5", 4.0f},					// CMD: crawl
	{BUZZARDAI_PLAYANIM, "tween2", 0.0f},				// CMD: from crawl
	{BUZZARDAI_BEAKYFLYHOME, "fly2", 45.0f},			// CMD: turn at x rads per second
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	backstrokeFly[] = 
{
	{BUZZARDAI_PLAYANIM, "tween3", 0.0f},				// CMD: into backstroke
	{BUZZARDAI_PLAYANIM, "fly6", 4.0f},					// CMD: backstroke
	{BUZZARDAI_PLAYANIM, "tween4", 0.0f},				// CMD: from backstroke
	{BUZZARDAI_BEAKYFLYHOME, "fly2", 45.0f},			// CMD: turn at x rads per second
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	beakyLand[] = 
{
	{BUZZARDAI_BEGINAPPROACH, "fly1", 60.0f},
	{BUZZARDAI_TURNHOME, "idle10", 0.0f},
	{BUZZARDAI_LAND, "fly2", 0.0f},
	{BUZZARDAI_END, NULL, 0.0f},
};

static BUZZARD_EVENT	beakyEggAttack[] = 
{
	{BUZZARDAI_EGGATTACK, "fly1", 3.0f},
	{BUZZARDAI_END, NULL, 0.0f},
};

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzardAIScripts
   Purpose : checks to see what any buzzards in the scene should be doing
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBuzzardAIScripts(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : RequestNewBuzzardScript
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/


int32 RequestNewBuzzardScript(ACTORINSTANCE *actorInstance, BUZZARD_EVENT *newScript);

#endif