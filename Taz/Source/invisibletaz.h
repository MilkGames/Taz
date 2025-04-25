#ifndef __H_INVISIBLETAZ
#define __H_INVISIBLETAZ

#define INVISIBLE_EFFECT_TIME		(25.0f)
#define INVISIBLE_INITIAL_TIME_DIFF	(0.75f)
#define INVISIBLE_NOT_TIME			(0.1f)

enum EPowerUpRenderStates
{
	RENDERSTATE_CARTOON,
	RENDERSTATE_OUTLINE,
	RENDERSTATE_LINEONLY,

	POWERUPSTATE_CHILLION,
	POWERUPSTATE_CHILLIOFF,
};

typedef struct TAG_POWERUPINFO
{
	TBVector	tempVector;

	float		clock;							//clock usde for time dependant power ups
	float		randomClock;					//clock used for random events
	float		thirdClock;						//another genral pupose clock
	int32		state;

	int			numFire;						//number of chillipepper fire left
	int			gumReady;						//is the gum ready to be used?
	int			gumBubbleHandle;				//handle for gum bubble attached to actor
	TBActorInstance	*gumBubble;					//gum bubble actorInstance structure ptr

	TBActorNodeInstance	*node;
	bool		hiccupJumpState;				// TP: true if character is currently doing a hiccup jump
	char		drawBubbles;
	char		chPad[3];
	int			iPad[2];

	int			invisible;						//is taz invisible
	int			hiccup;							//is taz in hiccup mode
	int			bubbleGum;						//is taz in bubblegum mode
	int			chilliPepper;					//is taz in chilli pepper mode
}POWERUPINFO, _POWERUPINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateCharacterInvisiblity(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: FinishInvisibility
	Purpose		: Turns off invisibility on an actorinstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FinishInvisibility(struct TAG_ACTORINSTANCE *actorInstance);

#endif