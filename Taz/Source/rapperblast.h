#ifndef	__H_RAPPERBLAST
#define __H_RAPPERBLAST

#include "actors.h"

enum
{
	RAPPER_MONO,
	RAPPER_STEREO,
	RAPPER_TRIPHONIC,
	RAPPER_QUADRAPHONIC,
};

#define		NUM_RAPPERBLASTSTREAKS		(4)
#define		NUM_RAPPERBLASTPOINTS		(20)
#define		RAPPERBLAST_MINRADIUS		(METERTOUNIT(2.0f))
#define		RAPPERBLAST_RADIUSINC		(METERTOUNIT(2.0f))
#define		RAPPERBLAST_AUDIOMODE		(RAPPER_STEREO)
#define		RAPPERBLAST_PERTAMP			(METERTOUNIT(0.15f))


typedef struct TAG_RAPPERBLASTSTREAK
{
	TBVector		points[NUM_RAPPERBLASTPOINTS];
	float			size[NUM_RAPPERBLASTPOINTS];
	int				colour[NUM_RAPPERBLASTPOINTS][4];
	float			alpha[NUM_RAPPERBLASTPOINTS];
} RAPPERBLASTSTREAK, _RAPPERBLASTSTREAK BALIGN16;

typedef struct TAG_RAPPERBLASTINFO
{
	RAPPERBLASTSTREAK			streaks[RAPPERBLAST_AUDIOMODE+1][NUM_RAPPERBLASTSTREAKS];

	float						minAngle, maxAngle;
	float						radius[NUM_RAPPERBLASTSTREAKS];

	TBActorNodeInstance			*speakers[4];

	float						clock;
	int32						draw;
	int32						iPod[2];
} RAPPERBLASTINFO, _RAPPERBLASTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : UpdateRapperBlast
   Purpose : creates waves of sound to blast enemies, should be called every frame while the attack is taking place
   Parameters : 
   Returns : 
   Info : 
*/

void CreateRapperBlast(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateRapperBlast
   Purpose : updates rapper blast effect
   Parameters : 
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void UpdateRapperBlast(ACTORINSTANCE *actorInstance);
	
/* --------------------------------------------------------------------------------
   Function : DrawRapperBlast
   Purpose : draws rapper blast effect
   Parameters : 
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void DrawRapperBlast(ACTORINSTANCE *actorInstance);

#endif