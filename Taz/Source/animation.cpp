// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : animation.cpp
//   Purpose : provides functions and structures to manage animations
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"
#include "global.h"

#include "actors.h"
#include "animation.h"
#include "buzzard.h"
#include "physics.h"
#include "attach.h"
#include "characters.h"

// PP: define DEBUG_ANIMS to get debug messages about animtaions
// PP: #define DEBUG_ANIMS


/* --------------------------------------------------------------------------------
   Function : InitActorAnimation
   Purpose : Initialises the animation part of an actorInstance 
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitActorAnimation(ACTOR_ANIMATION *actorAnimation)
{
	actorAnimation->status = 0;
	actorAnimation->timeSinceLastIdleMove = 0.0f;
	actorAnimation->timeInDanceState = 0.0f;
	actorAnimation->motion[X] = 0.0f;
	actorAnimation->motion[Y] = 0.0f;
	actorAnimation->motion[Z] = 0.0f;
	actorAnimation->motion[A] = 1.0f;
	actorAnimation->useMotionBone = TRUE;
	actorAnimation->lastIdle = -1;
	actorAnimation->waitingToEnd = WAITING_NONE;
	actorAnimation->flags = 0;
	actorAnimation->waterspoutClock = 0.0f;
}

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
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByOtherActorsName( ,%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	if (strcmpi(animationName, "animationName")==0)
	{
		int s  =78;
	}
	// check if animation name exists
	animation = baFindAnimSegment(&otherInstance->actorInstance, animationName);
	if (animation==NULL)
	{
//		bkPrintf("*** WARNING *** Could not find animation\n");
		return FALSE;
	}

	return PlayAnimationBySegmentPointer(actorInstance,animation,speed,loop,queue,blendTime,priority);
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByName
   Purpose : Request an animation by name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByName(ACTORINSTANCE *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByName(%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	animation = baFindAnimSegment(&actorInstance->actorInstance, animationName);
	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation %s\n",animationName);
		return FALSE;
	}

	return PlayAnimationBySegmentPointer(actorInstance,animation,speed,loop,queue,blendTime,priority);
}

/* --------------------------------------------------------------------------------
   Function : SetToStartFrame
   Purpose : Sets a queued anim to the first frame
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

void SetToStartFrame(ACTORINSTANCE *actorInstance, char *anim)
{
	baUpdateNodes(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,0,NULL);
	baPrepActorInstance(&actorInstance->actorInstance,true);
}

/*  --------------------------------------------------------------------------------
	Function	: SetToEndFrame
	Purpose		: Sets the instance to the last frame of the current animation
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void SetToEndFrame(ACTORINSTANCE *actorInstance, char *anim)
{
	uint32				frame;
	TBActorAnimSegment	*animSeg;

	FlushAnimationQueue(actorInstance);

	if (anim)
	{
		animSeg = baFindAnimSegment(&actorInstance->actorInstance, anim);
		if (!animSeg) 
		{
			bkPrintf("*** WARNING *** Could not find anim segment to set to end frame.\n");
			return;
		}
		frame = animSeg->endFrame;
	}
	else
	{
		return;
	}

	baSetNodeFrame(&actorInstance->actorInstance, NULL, frame, true);

	baUpdateNodes(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,0,NULL);
	baPrepActorInstance(&actorInstance->actorInstance,true);
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByCRCOfName
   Purpose : Request an animation by CRC of name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByCRCOfName(ACTORINSTANCE *actorInstance,uint32 nameCRC,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;

	// check if animation name exists
	animation = baFindAnimSegmentByCRC(&actorInstance->actorInstance, nameCRC);
	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation by CRC %d\n",nameCRC);
		return FALSE;
	}

	return PlayAnimationBySegmentPointer(actorInstance,animation,speed,loop,queue,blendTime,priority);
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByName
   Purpose : Request an animation by name, using a TBActorInstance
   Parameters : TBActorInstance, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByName(TBActorInstance *bActorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByName( ,%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	if (strcmpi(animationName, "gotopizza")==0)
	{
		int s  =78;
	}

	// check if animation name exists
	animation = baFindAnimSegment(bActorInstance, animationName);
	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation %s\n",animationName);
		return FALSE;
	}

	return PlayAnimationBySegmentPointer(bActorInstance,animation,speed,loop,queue,blendTime,priority);
}

/* --------------------------------------------------------------------------------
   Function : RandomlyPlayAnimationByName
   Purpose : Request an animation by name, using a TBActorInstance. Starts the anim in a random position
   Parameters : TBActorInstance, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : Used to stop the flockers all animating in the same way
*/
int RandomlyPlayAnimationByName(TBActorInstance *bActorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByName( ,%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	if (strcmpi(animationName, "gotopizza")==0)
	{
		int s  =78;
	}

	// check if animation name exists
	animation = baFindAnimSegment(bActorInstance, animationName);
	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation %s\n",animationName);
		return FALSE;
	}

	return RandomlyPlayAnimationBySegmentPointer(bActorInstance,animation,speed,loop,queue,blendTime,priority);
}


/* --------------------------------------------------------------------------------
   Function : PlayAnimationByNameAndSetCallback
   Purpose : Request an animation by name, using a TBActorInstance
   Parameters : TBActorInstance, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByNameAndSetCallback(TBActorInstance *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, TBQueuedAnimCallback callback, void *context)
{
	TBActorAnimSegment	*animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByNameAndSetCallback( ,%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	if (strcmpi(animationName, "gotopizza")==0)
	{
		int s  =78;
	}

	//Check Animation Name Exists
	animation = baFindAnimSegment(actorInstance, animationName);
	if(animation == NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation %s.\n",animationName);
		return FALSE;
	}

	return PlayAnimationBySegmentPointerAndSetCallback(actorInstance,animation,speed,loop,queue,blendTime,callback,context);
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointerAndSetCallback
   Purpose : Request an animation by segment pointer
   Parameters : ACTORINSTANCE, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointerAndSetCallback(TBActorInstance *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, TBQueuedAnimCallback callback, void *context)
{
	
#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationBySegmentPointerAndSetCallback( ,,%f,%d,%d,%f)\n",speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	if (animSegment==NULL)
	{
		bkPrintf("*** WARNING *** Invalid animation\n");
		return FALSE;
	}

	if (!queue)
	{
		// play immediately
		baFlushNodeQueue(actorInstance,actorInstance->rootNodeInstance,TRUE);
	}

	if (blendTime>0)
	{
		blendTime = 1/(blendTime*60.0f);
		// do blend animation
		baQueueNodeAnimBlendState(actorInstance, actorInstance->rootNodeInstance,
			animSegment, (int)(speed*1024.0f), -1, blendTime,loop>0?(BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING):0,
			callback,context);
	}
	else
	{
		// no blend required
		baQueueNodeAnimNormal(actorInstance, actorInstance->rootNodeInstance, 
			animSegment, (int)(speed*1024.0f), -1, loop>0?(BANIMQUEUEFLAG_LOOPING):0, callback,context);
	}

//	if (actorInstance->characterInfo) AnimateAttachedObjects(actorInstance,animSegment,(int)(speed*1024.0f),blendTime,loop,NULL);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationByNamePart
   Purpose : Request an animation by name
   Parameters : ACTORINSTANCE, animationName, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationByNamePart(ACTORINSTANCE *actorInstance,char *animationName,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority, TBActorNodeInstance *nodeInstance)
{
	TBActorAnimSegment *animation;

#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationByNamePart( ,%s,%f,%d,%d,%d)\n",animationName,speed,loop,queue,blendTime);
#endif

	if (strcmpi(animationName, "gotopizza")==0)
	{
		int s  =78;
	}
	// check if animation name exists
	animation = baFindAnimSegment(&actorInstance->actorInstance, animationName);
	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation\n");
		return FALSE;
	}

	return PlayAnimationBySegmentPointerPart(actorInstance,animation,speed,loop,queue,blendTime,priority,nodeInstance);
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointer
   Purpose : Request an animation by segment pointer
   Parameters : ACTORINSTANCE, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointer(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;
	int					currentPriority;
	
#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationBySegmentPointer( ,,%f,%d,%d,%f)\n",speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	animation = animSegment;

	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Invalid animation\n");
		return FALSE;
	}

	// check if animation has a high enough priority to play
	if (!queue)
	{
		currentPriority = GetPriorityOfCurrentAnimation(actorInstance);
		if (currentPriority > priority) 
		{
	#ifdef DEBUG_ANIMS
			bkPrintf("Animation too low priority\n");
	#endif
			return FALSE;
		}
	}

	if (!queue)
	{
		// play immediately
		if (actorInstance->characterInfo) FlushAttachedObjects(actorInstance);
		baFlushNodeQueue(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,TRUE);
	}

	if (blendTime>0)
	{
		blendTime = 1/(blendTime*60.0f);
		// do blend animation
		baQueueNodeAnimBlendState(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance,
			animation, (int)(speed*1024.0f), -1, blendTime,loop>0?(BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING):0,
			NULL,(void*)priority);
	}
	else
	{
		// no blend required
		baQueueNodeAnimNormal(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, 
			animation, (int)(speed*1024.0f), -1, loop>0?(BANIMQUEUEFLAG_LOOPING):0, NULL,(void*)priority);
	}

	if (actorInstance->characterInfo) AnimateAttachedObjects(actorInstance,animation,(int)(speed*1024.0f),blendTime,loop,(void*)priority);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointer
   Purpose : Request an animation by segment pointer
   Parameters : TBActorInstance, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointer(TBActorInstance *bActorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;
	int					currentPriority;
	
#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationBySegmentPointer( ,,%f,%d,%d,%f)\n",speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	animation = animSegment;

	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Invalid animation\n");
		return FALSE;
	}

	// check if animation has a high enough priority to play
	currentPriority = GetPriorityOfCurrentAnimation(bActorInstance);
	if (currentPriority > priority) 
	{
#ifdef DEBUG_ANIMS
		bkPrintf("Animation too low priority\n");
#endif
		return FALSE;
	}

	if (!queue)
	{
		// play immediately
		baFlushNodeQueue(bActorInstance,bActorInstance->rootNodeInstance,TRUE);
	}

	if (blendTime>0)
	{
		// do blend animation
		baQueueNodeAnimBlendState(bActorInstance, bActorInstance->rootNodeInstance,
			animation, (int)(speed*1024.0f), -1, 1/(blendTime*60.0f), loop>0?(BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING):0,
			NULL,(void*)priority);
	}
	else
	{
		// no blend required
		baQueueNodeAnimNormal(bActorInstance, bActorInstance->rootNodeInstance, 
			animation, (int)(speed*1024.0f), -1, loop>0?(BANIMQUEUEFLAG_LOOPING):0, NULL,(void*)priority);
	}
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : RandomlyPlayAnimationBySegmentPointer
   Purpose : Request an animation by segment pointer
   Parameters : TBActorInstance, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int RandomlyPlayAnimationBySegmentPointer(TBActorInstance *bActorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority)
{
	TBActorAnimSegment *animation;
	int					currentPriority;
	int					noofFrames;
	
#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationBySegmentPointer( ,,%f,%d,%d,%f)\n",speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	animation = animSegment;

	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Invalid animation\n");
		return FALSE;
	}

	// check if animation has a high enough priority to play
	currentPriority = GetPriorityOfCurrentAnimation(bActorInstance);
	if (currentPriority > priority) 
	{
#ifdef DEBUG_ANIMS
		bkPrintf("Animation too low priority\n");
#endif
		return FALSE;
	}

	if (!queue)
	{
		// play immediately
		baFlushNodeQueue(bActorInstance,bActorInstance->rootNodeInstance,TRUE);
	}

	noofFrames = animation->endFrame - animation->startFrame;

	if (blendTime>0)
	{

		// do blend animation
		baQueueNodeAnimBlendState(bActorInstance, bActorInstance->rootNodeInstance,
			animation, (int)(speed*1024.0f), (bmRand()%noofFrames)*1024, 1/(blendTime*60.0f), loop>0?(BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING):0,
			NULL,(void*)priority);
	}
	else
	{
		// no blend required
		baQueueNodeAnimNormal(bActorInstance, bActorInstance->rootNodeInstance, 
			animation, (int)(speed*1024.0f), (bmRand()%noofFrames)*1024, loop>0?(BANIMQUEUEFLAG_LOOPING):0, NULL,(void*)priority);
	}
	return TRUE;
}



/* --------------------------------------------------------------------------------
   Function : PlayAnimationBySegmentPointerPart
   Purpose : Request an animation by segment pointer from a part of the heirachy
   Parameters : ACTORINSTANCE, animation segment pointer, speed(1 normal),loop (YES/NO), queue (YES,NO)
				, blend time in seconds or 0 for immediate.
   Returns : 
   Info : 
*/
int PlayAnimationBySegmentPointerPart(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animSegment,float speed,char loop,char queue,
						float blendTime, EAnimationPriority priority, TBActorNodeInstance *nodeInstance)
{
	TBActorAnimSegment *animation;
	int					currentPriority;
	
#ifdef DEBUG_ANIMS
	bkPrintf("PlayAnimationBySegmentPointerPart( ,,%f,%d,%d,%f)\n",speed,loop,queue,blendTime);
#endif

	// check if animation name exists
	animation = animSegment;

	if (animation==NULL)
	{
		bkPrintf("*** WARNING *** Invalid animation\n");
		return FALSE;
	}

	// check if animation has a high enough priority to play
	currentPriority = GetPriorityOfCurrentAnimation(actorInstance);
	if (currentPriority > priority) 
	{
#ifdef DEBUG_ANIMS
		bkPrintf("Animation too low priority\n");
#endif
		return FALSE;
	}

	if (!queue)
	{
		// play immediately
		baFlushNodeQueue(&actorInstance->actorInstance,nodeInstance,TRUE);
	}

	if (blendTime>0)
	{
		// do blend animation
		if (loop)
		{
			baQueueNodeAnimBlendState(&actorInstance->actorInstance, nodeInstance,
				animation, (int)(speed*1024.0f), -1, 1/(blendTime*60.0f), BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING,
				NULL,(void*)priority);
//void baQueueNodeAnimBlendState(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *animSeg,
//							   int32 animDelta, int32 startFrame, float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context);
		}
		else
		{
			baQueueNodeAnimBlendState(&actorInstance->actorInstance, nodeInstance,
				animation, (int)(speed*1024.0f), -1, 1/(blendTime*60.0f), 0, NULL,(void*)priority);
		}
	}
	else
	{
		// no blend required
		if (loop)
		{
			baQueueNodeAnimNormal(&actorInstance->actorInstance, nodeInstance, 
					animation, (int)(speed*1024.0f), -1, BANIMQUEUEFLAG_LOOPING, NULL,(void*)priority);
		}
		else
		{
			baQueueNodeAnimNormal(&actorInstance->actorInstance, nodeInstance, 
					animation, (int)(speed*1024.0f), -1, 0, NULL,(void*)priority);
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CurrentAnimation
   Purpose : Request the current animaiton playing
   Parameters : ACTORINSTANCE
   Returns : the current segment being played or NULL if no anim being played
   Info : 
*/
TBActorAnimSegment *CurrentAnimation(ACTORINSTANCE *actorInstance)
{
	return CurrentAnimation(&actorInstance->actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : CurrentAnimation
   Purpose : Request the current animaiton playing
   Parameters : ACTORINSTANCE
   Returns : the current segment being played or NULL if no anim being played
   Info : 
*/
TBActorAnimSegment *CurrentAnimation(TBActorInstance *tbActorInstance)
{
	TBActorAnimSegment	*animSeg;
	int isPlaying;

	if(isPlaying = baIsActorAnimPlaying(tbActorInstance, tbActorInstance->rootNodeInstance,TRUE,&animSeg))
		return animSeg;
	else
		return NULL;

/*	if (tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].type == BANIMTYPE_NORMAL)
	{
		return tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].animSeg1;
	}
	else
	{
		return tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].animSeg1;
	}*/
}

/* --------------------------------------------------------------------------------
   Function : CheckIfPlayingAnimationPointedTo
   Purpose : Finds out if the actor instance is playing the specified animation
   Parameters : Babel actor instance ptr, (opt/NULL=any anim) animation to check for
   Returns : TRUE if the actor instance is playing the specified animation
   Info : // PP: I added this because it's handy
		// PP: couldn't have it overload CheckIfPlayingAnimation because a second parameter of NULL would make the call ambiguous
*/
BOOL CheckIfPlayingAnimationPointedTo(TBActorInstance* const tbActorInstance, const TBActorAnimSegment* const animation)
{
	if (animation==NULL)
	{
		return baIsActorAnimPlaying(tbActorInstance, tbActorInstance->rootNodeInstance, TRUE);
/*		if (!(tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].active))
			return FALSE;	// no animation playing
		else
			return TRUE;	// animation playing
		*/
	}

/*	if (!(tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].active))
	{
		return FALSE;
	}*/
	if (CurrentAnimation(tbActorInstance)!=animation)// am i already playing this anim
	{
		return FALSE;// no
	}
	else
	{
		return TRUE;// yes
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateActorAnimation
   Purpose : updates all the animation 
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void UpdateActorAnimation(ACTORINSTANCE *actorInstance)
{
	float	maxIceSlideSpeed = METERTOUNIT(10.0f);
	// increment the timeSinceLastIdleMove
	if (fps) 
	{
		actorInstance->actorAnimation->timeSinceLastIdleMove += 1.0f/fps;
		if (actorInstance->actorAnimation->useMotionBone)
		{
			if (actorInstance->actorBehaviour)
			{
				if(actorInstance->actorBehaviour->physics.currentTerrain & TERRAIN_ICE)
				{
					bmVectorCDiv(actorInstance->actorAnimation->motion, actorInstance->actorAnimation->motion, 10.0f);
					bmVectorAdd(actorInstance->actorBehaviour->physics.velocity, actorInstance->actorBehaviour->physics.velocity, actorInstance->actorAnimation->motion);
					if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > maxIceSlideSpeed)
					{
						bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity, actorInstance->actorBehaviour->physics.velocity, maxIceSlideSpeed);
					}
				}
				else
				if(actorInstance->special == SPECIALTYPE_BUZZARD)
				{
					SetActorVelocity(actorInstance->actorBehaviour,actorInstance->actorAnimation->motion);
				}
				else
				{
//					temp = bmVectorLen(actorInstance->actorAnimation->motion);
					actorInstance->actorAnimation->motion[Y] = actorInstance->actorBehaviour->physics.velocity[Y];// - (temp*0.1f);
					if(actorInstance->characterInfo)
					{
						if(actorInstance->characterInfo->powerUpInfo.bubbleGum)
						{
							actorInstance->actorAnimation->motion[Y] = actorInstance->characterInfo->powerUpInfo.tempVector[Y];
						}
					}
					SetActorVelocity(actorInstance->actorBehaviour,actorInstance->actorAnimation->motion);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : IsCurrentAnimationLooping
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : YES / NO
   Info : 
*/
BOOL IsCurrentAnimationLooping(ACTORINSTANCE *actorInstance)
{
	TBActorInstance *tbActorInstance = &actorInstance->actorInstance;

	if (tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].flags&BANIMQUEUEFLAG_LOOPING)
		return TRUE;
	if (tbActorInstance->rootNodeInstance->animQueue[tbActorInstance->rootNodeInstance->currentQueue].flags&BANIMQUEUEFLAG_LOOPPENDING)
		return TRUE;
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : GetPriorityOfCurrentAnimation
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : EAnimationPriority
   Info : 
*/
int GetPriorityOfCurrentAnimation(ACTORINSTANCE *actorInstance)
{
	return GetPriorityOfCurrentAnimation(&actorInstance->actorInstance);
}


/*  --------------------------------------------------------------------------------
	Function	: ClearCurrentAnimationPriority
	Purpose		: This function will clear the priority of the current animation being played
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void ClearCurrentAnimationPriority(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->actorInstance.rootNodeInstance->animQueue[actorInstance->actorInstance.rootNodeInstance->currentQueue].active)
	{
		actorInstance->actorInstance.rootNodeInstance->animQueue[actorInstance->actorInstance.rootNodeInstance->currentQueue].context = (void*)(NONE);
	}
}


/* --------------------------------------------------------------------------------
   Function : GetPriorityOfCurrentAnimation
   Purpose : like it sounds
   Parameters : ACTORINSTANCE
   Returns : EAnimationPriority
   Info : 
*/

int GetPriorityOfCurrentAnimation(TBActorInstance *bActorInstance)
{
	if (bActorInstance->rootNodeInstance->animQueue[bActorInstance->rootNodeInstance->currentQueue].active)
	{
		return (int) bActorInstance->rootNodeInstance->animQueue[bActorInstance->rootNodeInstance->currentQueue].context;
	}
	else
	{
		return NONE;
	}
}

/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void FlushAnimationQueue(ACTORINSTANCE *actorInstance, int flushFrom)
{
	baFlushNodeQueue(&actorInstance->actorInstance,ROOTNODE(actorInstance->actorInstance.rootNodeInstance),TRUE, flushFrom);
	FlushAttachedObjects(actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void FlushAnimationQueue(TBActorInstance *tbActorInstance,int flushFrom)
{

	baFlushNodeQueue(tbActorInstance,ROOTNODE(tbActorInstance->rootNodeInstance),TRUE, flushFrom);
}

/* --------------------------------------------------------------------------------
   Function : FlushAnimationQueue
   Purpose : stops all animations playing and wipes queue
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

int PlayFixedBlendOfTwoAnimations(TBActorInstance *actorInstance, char *animOne, char *animTwo, ANIMBLENDINFO *info, float blend, int flags, float speed)
{
	TBActorAnimSegment	*source;
	TBActorAnimSegment	*target;
	int					currSourceFrame;
	int					currTargetFrame, returnValue;
	float				segmentTime,currTime;

	source = baFindAnimSegment(actorInstance,animOne);
	target = baFindAnimSegment(actorInstance,animTwo);

	returnValue = FALSE;								// CMD: has the anim reached its end

	if((!source)||(!target))
	{
		bkPrintf("*** ERROR *** Could not find animation %s or %s for static blend!\n", animOne, animTwo);
		return FALSE;
	}

	//blend value must be between 0.0f and 1.0f
	if(blend < 0.0f) blend = 0.0f;
	if(blend > 1.0f) blend = 1.0f;
	//update anim
	if(flags & ANIMFLAG_UPDATE)
	{
		//source
		segmentTime = (float)(source->endFrame - source->startFrame)/(float)(/*source->ticksPerFrame*/60*1024);
		currTime = info->progress * segmentTime;
		currTime += fTime*speed;
		info->progress = currTime / segmentTime;
		if(info->progress > 1.0f)
		{
			if(flags & ANIMFLAG_LOOP)
			{
				info->progress -= 1.0f;
			}
			else
			{
				info->progress = 1.0f;
			}
			returnValue = TRUE;
		}

		currSourceFrame = (int)(info->progress * (float)(source->endFrame - source->startFrame)) + source->startFrame;
		//target
		currTargetFrame = (int)(info->progress * (float)(target->endFrame - target->startFrame)) + target->startFrame;
	}
	else
	if(info->progress)
	{
		//source
		currSourceFrame = (int)(info->progress * (float)(source->endFrame - source->startFrame)) + source->startFrame;
		//target
		currTargetFrame = (int)(info->progress * (float)(target->endFrame - target->startFrame)) + target->startFrame;
	}
	else
	{
		currSourceFrame = source->startFrame;
		currTargetFrame = target->startFrame;
	}

	// CMD: calculate smoothed blend
	if((info->currentBlend = info->currentBlend + (info->smooth*(blend - info->currentBlend))) > 0.9f) info->currentBlend = 1.0f;
	if(info->currentBlend < 0.1f) info->currentBlend = 0.0f;

	FlushAnimationQueue(actorInstance);

#ifdef DEBUG_ANIMS
	bkPrintf("FixedBlend\n");
#endif

	baQueueNodeAnimBlend(actorInstance,actorInstance->rootNodeInstance,source,target,0,0,currSourceFrame,currTargetFrame,0,0,NULL,NULL);
	baSetNodeBlendValue(actorInstance,actorInstance->rootNodeInstance,info->currentBlend,1);

	return returnValue;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnCurrentAnimation
	Purpose 	: sets a callback on the currently playing animation
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnCurrentAnimation(TBActorInstance *actorInstance, TBQueuedAnimCallback callback, void *context)
{
	// CMD: check for clear callback request
	if(callback == NULL)
	{
		actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].callback = NULL;
		actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].context = NULL;
		return TRUE;
	}
	if(!actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].callback)
	{
		actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].callback = callback;
		actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].context = context;
		return TRUE;
	}
	//animation already has callback
	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnLastAnimInQueue
	Purpose 	: sets a callback on the last animation in the queue
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnLastAnimInQueue(TBActorInstance *actorInstance, TBQueuedAnimCallback callback, void *context)
{
	int		currentAnim;
	
	currentAnim = actorInstance->rootNodeInstance->currentQueue;

	if(actorInstance->rootNodeInstance->animQueue[actorInstance->rootNodeInstance->currentQueue].active)
	{
		do
		{
			if(currentAnim >= (actorInstance->rootNodeInstance->queueSize-1)) currentAnim = 0;
			else currentAnim++;

		} while(actorInstance->rootNodeInstance->animQueue[currentAnim].active);

		if(--currentAnim < 0) currentAnim = actorInstance->rootNodeInstance->queueSize - 1;

		actorInstance->rootNodeInstance->animQueue[currentAnim].callback = callback;
		actorInstance->rootNodeInstance->animQueue[currentAnim].context = context;
		return TRUE;
	}
	else
	{
		//no animations queued
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SetCallbackOnNamedAnimation
	Purpose 	: sets a callback on the named animation if it is in the queue
	Parameters 	: 
	Returns 	: TRUE if completed successfully
	Info 		:
*/

int SetCallbackOnNamedAnimation(TBActorInstance *actorInstance, char *animName, TBQueuedAnimCallback callback, void *context)
{
	TBActorAnimSegment	*animSegment;

	animSegment = baFindAnimSegment(actorInstance, animName);
	if (animSegment==NULL)
	{
		bkPrintf("*** WARNING *** Could not find animation %s\n",animName);
		return FALSE;
	}

	//find correct anim in queue
	for(int i = (actorInstance->rootNodeInstance->queueSize-1);i >= 0; --i)
	{
		if(actorInstance->rootNodeInstance->animQueue[i].animSeg1 == animSegment)
		{
			actorInstance->rootNodeInstance->animQueue[i].callback = callback;
			actorInstance->rootNodeInstance->animQueue[i].context = context;
			return TRUE;
		}
	}
	// CMD: could not find anim in queue
	return FALSE;
}