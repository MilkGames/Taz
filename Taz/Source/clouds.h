#ifndef __H_CLOUDS
#define __H_CLOUDS


#include "maths.h"		// PP: maths stuff
#include "main.h"		// PP: main stuff		

#define	DRAW_CLOUDS_IN_BACKGROUND	(1<<0)
#define	DRAW_CLOUDS_IN_FOREGROUND	(1<<1)
#define	DRAW_CLOUDS_FIXED_HEIGHT	(1<<2)

enum
{
	SKYCLOUD_FLUFFY,
	SKYCLOUD_WACKY,
	PLAYER_STARS,
};

typedef struct TAG_SKYCLOUD
{
	TBActorInstance	cloudInstance;

	TBVector		offset;

	TBVector		rotationAxis;

	float			initialHeight;
	float			initialRadius;
	float			angularVelocity;
	float			initialAngle;

	float			currentAngle;
	uint32			type;
	TAG_SKYCLOUD	*next;
	TAG_SKYCLOUD	*prev;

	int				alpha;
	uint32			drawFlags;
	struct TAG_ACTORINSTANCE	*followActor;
	TBActorNodeInstance		*followNode;
} SKYCLOUD, _SKYCLOUD BALIGN16;

typedef struct TAG_CLOUDBANK
{
	SKYCLOUD			head;

	uint32				numEntries;
	uint32				pad[3];
} CLOUDBANK, _CLOUDBANK BALIGN16;

extern CLOUDBANK		cloudBank;

/* --------------------------------------------------------------------------------
   Function : InitialiseCloudBank
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseCloudBank(void);

/* --------------------------------------------------------------------------------
   Function : CreateCloudsInSky
   Purpose : creates clouds in the sky, doh!
   Parameters : bounds[0]=minRadius, bounds[1]=maxRadius, bounds[2]=minHeight, bounds[3]=maxHeight
   Returns : 
   Info : 
*/

int CreateCloudsInSky(TBVector bounds, int number, float speed, int maxAlpha, int minAlpha, uint32 type, struct TAG_ACTORINSTANCE *followActor, TBActorNodeInstance *followNode, TBVector offset);

/* --------------------------------------------------------------------------------
   Function : AddEntryToCloudBank
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

SKYCLOUD *AddEntryToCloudBank(void);

/* --------------------------------------------------------------------------------
   Function : AddEntryToCloudBank
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

int DeleteEntryFromCloudBank(SKYCLOUD *remPtr);

/* --------------------------------------------------------------------------------
   Function :
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void UpdateCloudsInSky(void);

/* --------------------------------------------------------------------------------
   Function :FreeCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void FreeCloudBank();

/* --------------------------------------------------------------------------------
   Function :PrepCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void PrepCloudBank(void);

/* --------------------------------------------------------------------------------
   Function :DrawCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DrawCloudBankBackground(void);

/* --------------------------------------------------------------------------------
   Function :DrawCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DrawCloudBankForeground(void);

/* --------------------------------------------------------------------------------
   Function :CreateInjuryStars
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void CreateInjuryStars(int number, struct TAG_ACTORINSTANCE *actorInstance, char *bone, TBVector offset, TBVector bounds);

/* --------------------------------------------------------------------------------
   Function		: CreateInjuryStars
   Purpose		: Create standard injury stars
   Parameters	: ptr to ACTORINSTANCE for whom to create stars
   Returns		: 
   Info			: // PP: 
*/
#define INJURYSTARS_DEFAULT_ACTORINSTANCE		(gameStatus.player1)	// PP: default ACTORINSTANCE for whom to create injury stars
#define INJURYSTARS_DEFAULT_Y_OFFSET			METERTOUNIT(0.2f)		// PP: default Y offset for injury stars

void CreateInjuryStars(ACTORINSTANCE* const actorInstance = INJURYSTARS_DEFAULT_ACTORINSTANCE,
					   const float yOffset = INJURYSTARS_DEFAULT_Y_OFFSET);

/* --------------------------------------------------------------------------------
   Function :DestroyInjuryStars
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DestroyInjuryStars(struct TAG_ACTORINSTANCE *actorInstance);

#endif