#ifndef __H_ANIMATION
#define __H_ANIMATION

#include "actors.h"

#define ANIMFLAG_UPDATE	(1<<0)
#define ANIMFLAG_LOOP	(1<<1)

// CMD: general info flags
#define	ANIMFLAG_WATERSPOUT	(1<<2)

// PP: number of frames in an anim
#define ANIMFRAMES(_anim)		(((_anim)->endFrame-(_anim)->startFrame)/BTICKSPERFRAME)

// PP: length in seconds of an anim 
#define ANIMTIME(_anim)			(ANIMFRAMES(_anim)/60.0f)

enum
{
	WAITING_NONE,
	WAITING_CANNONFAIL,
	WAITING_WATERBOUNCE,
};

typedef struct TAG_ACTOR_ANIMATION
{
	TBVector			motion;						// contains the motion bone information

	int32				status;						// contains current status of animation
	float				timeSinceLastIdleMove;		// contains the time in seconds since the last idle
	float				timeInDanceState;			// time, in seconds, that taz has been dancing
	int32				useMotionBone;				// Enables and disbles the use of motion bone

	int					lastIdle;					// number corresponding to last idle animation used
	int					waitingToEnd;
	float				waterspoutClock;
	uint32				flags;
} ACTOR_ANIMATION, _ACTOR_ANIMATION;


enum EAnimationPriority	{
	NONE,
	IDLE,
	MOVE,
	TIPTOE,
	SLIDESTOP,
	JUMP,
	HICCUP,
	ATTACK,
	REACT,
	DEATH,
};

typedef struct TAG_ANIMBLENDINFO
{
	float		progress;
	float		currentBlend;
	float		smooth;
}ANIMBLENDINFO, _ANIMBLENDINFO;

/* --------------------------------------------------------------------------------
   Function : InitActorAnimation
   Purpose : Initialises the animation part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitActorAnimation(ACTOR_ANIMATION *actorAnimation);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByOtherActorsName
   Purpose : Request an animation by name using the segment info from another actor
   Parameters : ACTORINSTANCE, actorInstance to take anim segment from,
				animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByOtherActorsName(ACTORINSTANCE *actorInstance,ACTORINSTANCE *otherInstance, char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByName
   Purpose : Request an animation by name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds, priority
   Returns : 
   Info : 
*/
int PlayAnimationByName(ACTORINSTANCE *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByCRCOfName
   Purpose : Request an animation by CRC of name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByCRCOfName(ACTORINSTANCE *actorInstance,uint32 nameCRC,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByName
   Purpose : Request an animation by name, using a TBActorInstance
   Parameters : TBActorInstance, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByName(TBActorInstance *bActorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);
// IH: Lets get the flocking anims out of sync
int RandomlyPlayAnimationByName(TBActorInstance *bActorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);


/* --------------------------------------------------------------------------------
   Function : PlayAnimationByNameAndSetCallback
   Purpose : Request an animation by name, using a TBActorInstance
   Parameters : TBActorInstance, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByNameAndSetCallback(TBActorInstance *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, TBQueuedAnimCallback callback, void *context);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointerAndSetCallback
   Purpose : Request an animation by segment pointer
   Parameters : TBActorInstance, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointerAndSetCallback(TBActorInstance *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, TBQueuedAnimCallback callback, void *context);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByNamePart
   Purpose : Request an animation by name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByNamePart(ACTORINSTANCE *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority, TBActorNodeInstance *nodeInstance);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointer
   Purpose : Request an animation by segment pointer
   Parameters : ACTORINSTANCE, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointer(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointer
   Purpose : Request an animation by segment pointer
   Parameters : TBActorInstance, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointer(TBActorInstance *bActorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);
// IH: For the flockers
int RandomlyPlayAnimationBySegmentPointer(TBActorInstance *bActorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority);


/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointerPart
   Purpose : Request an animation by segment pointer from a part of the heirachy
   Parameters : ACTORINSTANCE, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointerPart(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority, TBActorNodeInstance *nodeInstance);

/* --------------------------------------------------------------------------------
   Function : CurrentAnimation
   Purpose : Request the current animaiton playing
   Parameters : ACTORINSTANCE
   Returns : the current segment being played or NULL if no anim being played
   Info : 
*/
TBActorAnimSegment *CurrentAnimation(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CurrentAnimation
   Purpose : Request the current animaiton playing
   Parameters : ACTORINSTANCE
   Returns : the current segment being played or NULL if no anim being played
   Info : 
*/
TBActorAnimSegment *CurrentAnimation(TBActorInstance *tbActorInstance);

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimationPointedTo
   Purpose : Finds out if the actor instance is playing the specified animation
   Parameters : Babel actor instance ptr, (opt/NULL=any anim) animation to check for
   Returns : TRUE if the actor instance is playing the specified animation
   Info : // PP: I added this because it's handy
		// PP: couldn't have it overload CheckIfPlayingAnimation because a second parameter of NULL would make the call ambiguous
*/
BOOL CheckIfPlayingAnimationPointedTo(TBActorInstance* const tbActorInstance, const TBActorAnimSegment* const animation=NULL);

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimation
   Purpose : Finds out if the current animation is the same
   Parameters : Babel actor instance ptr, name of animation to test for
   Returns : TRUE if already playing
   Info : // PP: I made this inline to because it now does little more than call another function
*/
inline BOOL CheckIfPlayingAnimation(TBActorInstance *tbActorInstance, char *animName)
{
	TBActorAnimSegment *animation;

	if(animName == NULL)
	{
		animation=NULL;
	}
	else
	{
		animation=baFindAnimSegment(tbActorInstance, animName);
		if (!animation) return FALSE;
	}

	return CheckIfPlayingAnimationPointedTo(tbActorInstance, animation);
}

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimation
   Purpose : Finds out if the current animation is the same
   Parameters : ACTORINSTANCE, name of animation to test for
   Returns : TRUE if already playing
   Info : // PP: I made this inline to because it now does nothing more than call another function
*/
inline BOOL CheckIfPlayingAnimation(ACTORINSTANCE *actorInstance, char *animName)
{
	return CheckIfPlayingAnimation(&(actorInstance->actorInstance), animName);
}

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimation
   Purpose : Finds out if the current animation is the same
   Parameters : Babel actor instance ptr, name of animation to test for
   Returns : TRUE if already playing
   Info : // PP: I made this inline to because it now does little more than call another function
*/
inline BOOL CheckIfPlayingAnimation(TBActorInstance *tbActorInstance, char *animName, uint32 animNameCRC)
{
	TBActorAnimSegment *animation;

	if(!animNameCRC)
	{
		animation=NULL;
	}
	else
	{
		animation=baFindAnimSegmentByCRC(tbActorInstance, animNameCRC);
		if (!animation) return FALSE;
	}

	return CheckIfPlayingAnimationPointedTo(tbActorInstance, animation);
}

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimation
   Purpose : Finds out if the current animation is the same
   Parameters : ACTORINSTANCE, name of animation to test for
   Returns : TRUE if already playing
   Info : // PP: I made this inline to because it now does nothing more than call another function
*/
inline BOOL CheckIfPlayingAnimation(ACTORINSTANCE *actorInstance, char *animName, uint32 animNameCRC)
{
	return CheckIfPlayingAnimation(&(actorInstance->actorInstance), animName, animNameCRC);
}

/* --------------------------------------------------------------------------------
   Function : UpdateActorAnimation
   Purpose : updates all the animation 
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void UpdateActorAnimation(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : IsCurrentAnimationLooping
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : YES / NO
   Info : 
*/
BOOL IsCurrentAnimationLooping(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : GetPriorityOfCurrentAnimation
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : EAnimationPriority
   Info : 
*/
int GetPriorityOfCurrentAnimation(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : GetPriorityOfCurrentAnimation
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : EAnimationPriority
   Info : 
*/
int GetPriorityOfCurrentAnimation(TBActorInstance *bActorInstance);

/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void FlushAnimationQueue(ACTORINSTANCE *actorInstance,int flushFrom=0);

/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void FlushAnimationQueue(TBActorInstance *tbActorinstance,int flushFrom=0);

/* --------------------------------------------------------------------------------
   Function : SetToStartFrame
   Purpose : Sets a queued anim to the first frame
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

void SetToStartFrame(ACTORINSTANCE *actorInstance, char *anim=NULL);

/*  --------------------------------------------------------------------------------
	Function	: SetToEndFrame
	Purpose		: Sets the instance to the last frame of the current animation
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void SetToEndFrame(ACTORINSTANCE *actorInstance, char *anim=NULL);


/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

int PlayFixedBlendOfTwoAnimations(TBActorInstance *actorInstance, char *animOne, char *animTwo, ANIMBLENDINFO *info, float blend, int flags, float speed=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnCurrentAnimation
	Purpose 	: sets a callback on the currently playing animation
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnCurrentAnimation(TBActorInstance *actorInstance, TBQueuedAnimCallback callback, void *context);

/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnLastAnimInQueue
	Purpose 	: sets a callback on the last animation in the queue
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnLastAnimInQueue(TBActorInstance *actorInstance, TBQueuedAnimCallback callback, void *context);

/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnNamedAnimation
	Purpose 	: sets a callback on the named animation if it is in the queue
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnNamedAnimation(TBActorInstance *actorInstance, char *animName, TBQueuedAnimCallback callback, void *context);

/*  --------------------------------------------------------------------------------
	Function	: ClearCurrentAnimationPriority
	Purpose		: This function will clear the priority of the current animation being played
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void ClearCurrentAnimationPriority(ACTORINSTANCE *actorInstance);

#endif