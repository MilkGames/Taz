#ifndef __H_SUPERBURPEFFECT
#define __H_SUPERBURPEFFECT

#include "debris.h"

#define	BLASTRADIUS_VELOCITY	(4.0f)
#define BLASTRADIUS_ALPHADECAY	(20.0f)
#define MAX_BUBBLE_SPRITES		(1)
#define BURPBUBBLE_LIFE			(3.0f)

enum EBubbleColour
{
	GREEN_BUBBLE,
	WHITE_BUBBLE,
};

typedef struct TAG_BLASTRADIUSINFO
{
	TBActorInstance		blastActorInstance;

	float				alpha;
	float				radius;
	int					iPad[2];
} BLASTRADIUSINFO, _BLASTRADIUSINFO BALIGN16;

typedef struct TAG_BUBBLESINFO
{
} BUBBLESINFO, _BUBBLESINFO BALIGN16;

typedef struct TAG_SUPERBURPEFFECT
{
	BLASTRADIUSINFO		*blastRadiusInfo;
	BUBBLESINFO			*bubblesInfo;							// CMD: no, this is not related to monkeys!
	float				bubblesClock;
	int					soundHandle;							// TP: used to control the looping sound effect
	float				clock;
} SUPERBURPEFFECT, _SUPERBURPEFFECT BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateSuperBurpBlastRadius
   Purpose : Creates structure for blast radius effect
   Parameters : actorInstance that has superburped
   Returns : true/false depending on success
   Info : the capital of Surinam is Paramarabou
*/

int CreateSuperBurpBlastRadius(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : PrepSuperBurpBlastRadius
   Purpose : preps and frees blast radius effect
   Parameters : actorInstance that has superburped
   Returns : TRUE if exists
   Info : I have been reading about C++ recently, and it is actually a bit poo
*/

int PrepSuperBurpBlastRadius(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateSuperBurpBlastRadius
   Purpose : updates blast radius effect
   Parameters : actorInstance that has superburped
   Returns : 
   Info : women live longer than men, except those shot by their husbands
*/

void UpdateSuperBurpBlastRadius(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawSuperBurpBlastRadius
   Purpose : Draw blast radius effect
   Parameters : actorInstance that has superburped
   Returns : 
   Info : Louise Redknapp is in fact less than 2Inches tall, which must mean that Jamie is also very small.
*/

void DrawSuperBurpBlastRadius(struct TAG_ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: CreateBubbles
	Purpose 	: Creates bubbles from taz's mouth when in superburp mode
	Parameters 	: 
	Returns 	: 
	Info 		: Mint Aero's are very nice, however thay are not recomended for after a largeish lunch.
*/

void CreateBubbles(struct TAG_ACTORINSTANCE *actorInstance,float maxScale, float life, EBubbleColour colour);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateBubbles
	Purpose 	: updates bubbles
	Parameters 	: debris data structure
	Returns 	: 
	Info 		: thinking up usefull info at short notice can be quite difficult
*/

int32 UpdateBubbles(BUBBLESEXTRA *data);

#endif