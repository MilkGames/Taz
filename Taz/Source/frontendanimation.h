#ifndef __H_FRONTENDANIMATION
#define __H_FRONTENDANIMATION

#include "status.h"
#include "animation.h"
#include "frontendmenu.h"

#define FEPATH_THERE		(0)
#define FEPATH_BACK			(1)

enum EFrontEndIncedentals
{
	FE_GRAMOPHONE,
	FE_ALARMCLOCK,
	FE_TNTPLUNGER,
	FE_TELEVISION,
	FE_PIZZAS,
	FE_DECKCHAIR,
	FE_CHEST,
	FE_INITCAMERA,

	NUM_FE_INCEDENTALS,
};

enum EFrontEndSavePaths
{
	SLOT_ONE_TO_TWO,
	SLOT_TWO_TO_THREE,
	SLOT_THREE_TO_TWO,
	SLOT_TWO_TO_ONE,
	SLOT_ONE_TO_GAME,
	SLOT_TWO_TO_GAME,
	SLOT_THREE_TO_GAME,

	NUM_SAVE_SLOT_PATHS,
};

typedef struct TAG_FRONTENDANIMINFO
{
	ACTORINSTANCE		*paths[MENU_NUMSUBMENUS][2];
	ACTORINSTANCE		*incedentals[NUM_FE_INCEDENTALS];
	ACTORINSTANCE		*savePaths[NUM_SAVE_SLOT_PATHS];
	char				animRequest[64];

	EMenuPage			lastMenu;
	EMenuPage			targetMenu;
	ANIMBLENDINFO		animInfo;
	ANIMBLENDINFO		tntBlendInfo;

	int					playingInitialAnim;
	int					delayOver;
	EFrontEndSavePaths	currentSavePath;
	EMainAnimState		currentState;

	EMainAnimState		requestState;
	EMainAnimState		lastState;
	int32				iPad[2];	
}FRONTENDANIMINFO, _FRONTENDANIMINFO BALIGN16;

extern FRONTENDANIMINFO	frontEndAnimInfo;

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseFrontEndAnimationPaths
	Purpose 	: initialises paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseFrontEndAnimationPaths(void);

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseFrontEndAnimationPaths
	Purpose 	: initialises paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int ChooseFrontEndAnimationPath(EMenuPage target);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndAnimPaths
	Purpose 	: updates paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndAnimPaths(void);

/*	--------------------------------------------------------------------------------
	Function 	: RequestFrontEndStateChange
	Purpose 	: changes taz's front end state
	Parameters 	: .3

	Returns 	: 
	Info 		:
*/

int RequestFrontEndStateChange(ECharState newState);

/*	--------------------------------------------------------------------------------
	Function 	: ResetFrontEndAnimPath
	Purpose 	: changes taz's front end state
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ResetFrontEndAnimPath(struct TAG_ACTORINSTANCE *actorInstance, char *animName);

/*	--------------------------------------------------------------------------------
	Function 	: RequestTazFrontEndAnimation
	Purpose 	: puts in request for animation
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RequestTazFrontEndAnimation(char *animName);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTazFrontEndAnimation
	Purpose 	: check to see if an anim can play, and what action need to be taken
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateTazFrontEndAnimation(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateIncedentalsFrontEndAnimation
	Purpose 	: check to see if an anim can play, and what action need to be taken
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateIncedentalsFrontEndAnimation(void);

/*	--------------------------------------------------------------------------------
	Function 	: ChooseIdleAnimationForTazInFrontEnd
	Purpose 	: chooses an idle anim for taz from list at start of frontendanimation.cpp
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

TBActorAnimSegment *ChooseIdleAnimationForTazInFrontEnd(struct TAG_ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: FrontEndRotateTazCallback
	Purpose 	: rotates taz upon completion of animation
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

int FrontEndRotateTazCallback(EBQueueCallbackReason reasonCode,TBActorInstance *actor,TBActorNodeInstance *node,
							  TBActorAnimSegment *segment,void *context);

/*	--------------------------------------------------------------------------------
	Function 	: FrontEndPlunge
	Purpose 	: Plunges the plunger (Well what else?)
	Parameters 	: 
	Returns 	:
	Info 		:
*/
int FrontEndPlunge(void);

#endif