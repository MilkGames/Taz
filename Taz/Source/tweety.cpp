// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tweety.cpp
//   Purpose : Tweety AI
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "tweety.h"
#include "scene.h"
#include "mapfile.h"
#include "main.h"
#include "maths.h"
#include "animation.h"
#include "status.h"
#include "files.h"
#include "trail.h"
#include "control.h"
#include "camera.h"
#include "subtitle.h"

TWEETY	tweety;

enum TWEETYINTROSTAGES
{
	Off,
	Setup,
	Enter,
	Complain,
	Explain,
	Retreat,
} tweetyIntroStage;

bool interuptTweetyIntro(float time, float duration);

/* --------------------------------------------------------------------------------
   Function : CreateTweety
   Purpose : Create, initialise and return a tweety
   Parameters : start position
   Returns : 
   Info : tweety.obe must be loaded before this function is called
*/
void CreateTweety(TBVector start,TBQuaternion quat)
{
	TBActor		*actor;

	tweety.actorInstance = NULL;

	// TP: find and create tweety
	actor = FindActorInActorList("tweety.obe");
	if (!actor)
	{
		bkPrintf("*** ERROR *** Could not create tweety\n");
		return;
	}

	tweety.actorInstance = CreateActorInstance(&map.animatedInstances,"tweety.obe","tweety");
	EnableActorCartoonRenderMode(&tweety.actorInstance->actorInstance);

	tweety.actorInstance->flags |= ACTORFLAG_CUSTOMDRAW;

	bmVectorCopy(tweety.destPosition,start);
	
	bmVectorCopy(tweety.actorInstance->actorInstance.position,start);

	if (map.sceneNumber == SCENE_LANGUAGESELECT)
	{
		tweety.destPosition[Y] = METERTOUNIT(2.0f);

		tweety.actorInstance->actorInstance.position[X] = 2.0;
		tweety.actorInstance->actorInstance.position[Z] = 2.0;
	}
	else
	{
		tweety.destPosition[Y] += METERTOUNIT(1.0f);
	}
	
	bmQuatCopy(tweety.destQuat,quat);

	if (!tweety.actorInstance)
	{
		bkPrintf("*** ERROR *** Could not create tweety\n");
		return;
	} 

	tweety.state = TWEETYSTATE_HOVER;

	ZEROVECTOR(tweety.vel);

	return;
}

BOOL	TweetyTalking = false;

float	tweetyTimer = 0.0f;

void StartTweetyIntro()
{
	tweetyIntroStage = Setup;

	TBVector	tempVector;
	TBMatrix	tempMatrix;
						
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(1.5f),1.0f);

	bmQuatToMatrix(tempMatrix,gameStatus.player1->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
								
	bmVectorAdd(tempVector,gameStatus.player1->actorInstance.position,tempVector);
							
	tempVector[Y] -= METERTOUNIT(1.0f);
								
	TeleportTweety(tempVector);

	SetTweetyAOI(gameStatus.player1);

	TweetyTalking = false;
}

void DoTweetyIntro()
{
	if (tweetyIntroStage == Off)
		return;

	switch (tweetyIntroStage)
	{
	case Setup:		if (!TweetyTalking)
					{
						tweetyTimer = gameStatus.gameTime;
						TweetyTalking = true;
					}
		
					if (/*interuptTweetyIntro(tweetyTimer, 1.5f)*/(gameStatus.player1->actorStatus->currentState!=STATE_RUNON)
						&&(gameStatus.player1->actorStatus->currentState!=STATE_INIT))
					{
						TweetyTalking = false;

						tweetyIntroStage = Enter;

						gameStatus.controlSuspend = true;
					}
		
					break;

	case Enter:		if (!TweetyTalking)
					{
						Subtitle(STR_ZOOHUBINTRO2_17, 3.0f);
						tweetyTimer = gameStatus.gameTime;
						TweetyTalking = true;
					}

					if (interuptTweetyIntro(tweetyTimer, 3.5f))
					{
						CloseCurrentSubtitle();

						StopChannel(&characterSounds.tweety);

						tweetyIntroStage = Complain;
						TweetyTalking = false;

						FORCE_OKBUTTON_DEBOUNCE;
					}

					break;

	case Complain:	if (!TweetyTalking)
					{
						Subtitle(STR_ZOOHUBINTRO2_18, 4.0f);
						tweetyTimer = gameStatus.gameTime;
						TweetyTalking = true;
					}

					if (interuptTweetyIntro(tweetyTimer, 4.5f))
					{
						CloseCurrentSubtitle();

						StopChannel(&characterSounds.tweety);
						
						tweetyIntroStage = Explain;
						TweetyTalking = false;

						FORCE_OKBUTTON_DEBOUNCE;
					}

					break;

	case Explain:	if (!TweetyTalking)
					{
						Subtitle(STR_ZOOHUBINTRO2_19, 6.0f);
						tweetyTimer = gameStatus.gameTime;
						TweetyTalking = true;
					}

					if (interuptTweetyIntro(tweetyTimer, 6.5f))
					{
						CloseCurrentSubtitle();

						StopChannel(&characterSounds.tweety);
												
						tweetyIntroStage = Retreat;
						TweetyTalking = false;

						SetTweetyAOI(FindActorInstanceInMap("tweety trigger", 0, &map));

						FORCE_OKBUTTON_DEBOUNCE;
					}

					break;
	
	case Retreat:	if (!TweetyTalking)
					{
						Subtitle(STR_ZOOHUBINTRO2_20, 2.0f);
						tweetyTimer = gameStatus.gameTime;
						TweetyTalking = true;
					}

					if (interuptTweetyIntro(tweetyTimer, 2.5f))
					{
						CloseCurrentSubtitle();

						StopChannel(&characterSounds.tweety);
						
						tweetyIntroStage = Off;
						TweetyTalking = false;
						gameStatus.controlSuspend = false;
						
						FORCE_OKBUTTON_DEBOUNCE;
					}

					break;
	
	case Off:		return;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ResetTweetyPosition
	Purpose		: Reset tweety's position next to Taz
	Parameters	: position, quat
	Returns		: 
	Info		: 
*/
void ResetTweetyPosition(TBVector position, TBQuaternion quat)
{
	if (!tweety.actorInstance) return;
	bmVectorCopy(tweety.destPosition,position);
	tweety.destPosition[Y] += METERTOUNIT(1.0f);
	bmVectorCopy(tweety.actorInstance->actorInstance.position,position);
	bmQuatCopy(tweety.destQuat,quat);
}

void TweetyLookAt2D(TBVector point)
{
	TBVector	vec;

	bmVectorSub(vec,point,tweety.actorInstance->actorInstance.position);
	vec[Y] = 0;
	vec[W] = 1.0f;
	if ((vec[X] == 0.0f)&&(vec[Z] == 0.0f)) return;
	bmDirectionToQuat(tweety.destQuat,vec,0.0f);
}

/* --------------------------------------------------------------------------------
   Function : MoveTweety
   Purpose : Move tweety towards destination avoiding obsticles (Yer right!)
   Parameters : time of update
   Returns : 
   Info : 
*/
float			accel = 5.0f;
float			drag = 0.01f;
	
void MoveTweety(void)
{
	TBVector		tempVector;

	float			speed,tempFloat;

	bmVectorSub(tempVector,tweety.destPosition,tweety.actorInstance->actorInstance.position);
	tempFloat = bmVectorLen(tempVector);
	
	if (tempFloat)
	{
		bmVectorCMul(tempVector,tempVector,accel/tempFloat);
	}
	else
	{
		ZEROVECTOR(tempVector);
	}

	bmVectorCMul(tempVector,tempVector,accel);

	bmVectorAdd(tweety.vel,tweety.vel,tempVector);

	speed = bmVectorLen(tweety.vel);
	if ((speed > 0.0f)&&(speed > METERTOUNIT(30.0f)))
	{
		bmVectorScaleToLength(tweety.vel,tweety.vel,METERTOUNIT(30.0f));
	}

	bmVectorCMul(tempVector,tweety.vel,fTime);
	bmVectorAdd(tweety.actorInstance->actorInstance.position,tweety.actorInstance->actorInstance.position,tempVector);

	// TP: Drag
	bmVectorCMul(tempVector,tweety.vel,drag);
	bmVectorSub(tweety.vel,tweety.vel,tempVector);

	// TP: rotate tweety
	TBQuaternion tempQuat;
	bmQuatCopy(tempQuat, tweety.actorInstance->actorInstance.orientation);
	bmQuatSlerpAtFixedSpeed(tweety.actorInstance->actorInstance.orientation,tempQuat,tweety.destQuat,0.05f*speedRatio);
}

/*
Idle1
Flyforward
Flyleft
Flyright
Flyback
Laugh
Gesture1
Gesture2
*/
void AnimateTweety(void)
{
	float		facing,travel;
	TBVector	tempVector;
	float		angle;
	char		quad;

	enum
	{
		FRONT,
		RIGHT,
		BACK,
		LEFT,
	};

	if (tweety.actorInstance==NULL) return;

	if (tweety.state == TWEETYSTATE_LAUGH)
	{
		if (!CheckIfPlayingAnimation(tweety.actorInstance, "laugh"))
		{
			FlushAnimationQueue(tweety.actorInstance);
			PlayAnimationByName(tweety.actorInstance,"laugh",1.0f,1,0,0.3f,NONE);
		}
	}
	else if (tweety.state == TWEETYSTATE_SHORTLAUGH)
	{
		if (!CheckIfPlayingAnimation(tweety.actorInstance, "laugh"))
		{
			tweety.state = TWEETYSTATE_HOVER;
		}
	}
	else
	{
		// TP: current facing of tweety
		facing = GetAngleAroundYFromQuat(tweety.actorInstance->actorInstance.orientation);

		// TP: current direction of movement
		bmVectorSub(tempVector,tweety.actorInstance->actorInstance.position,tweety.destPosition);
		travel = GetAngleAroundYFromVector(tempVector);

		angle = FindShortestAngleSigned(facing,travel);

		angle = Aabs(angle + RAD(45.0f));

		if (angle < RAD(90.0f))	quad = FRONT;// TP: forward
		else if (angle < RAD(180.0f))	quad = RIGHT;// TP: right
		else if (angle < RAD(270.0f))	quad = BACK;// TP: backwards
		else quad = LEFT;// TP: left

		switch(quad)
		{
		case FRONT:
			if (!CheckIfPlayingAnimation(tweety.actorInstance,"fly1"))
			{
				FlushAnimationQueue(tweety.actorInstance);
				PlayAnimationByName(tweety.actorInstance,"fly1",1.0f,1,0,0.3f,NONE);
			}
			break;
		case RIGHT:
			if (!CheckIfPlayingAnimation(tweety.actorInstance,"flyleft"))
			{
				FlushAnimationQueue(tweety.actorInstance);
				PlayAnimationByName(tweety.actorInstance,"flyleft",1.0f,1,0,0.3f,NONE);
			}
			break;
		case BACK:
			if (!CheckIfPlayingAnimation(tweety.actorInstance,"flyback"))
			{
				FlushAnimationQueue(tweety.actorInstance);
				PlayAnimationByName(tweety.actorInstance,"flyback",1.0f,1,0,0.3f,NONE);
			}
			break;
		case LEFT:
			if (!CheckIfPlayingAnimation(tweety.actorInstance,"flyright"))
			{
				FlushAnimationQueue(tweety.actorInstance);
				PlayAnimationByName(tweety.actorInstance,"flyright",1.0f,1,0,0.3f,NONE);
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SetTweetyDest
   Purpose : Set Tweety's destination point
   Parameters : 
   Returns : 
   Info : 
*/

void SetTweetyDest(TBVector dest)
{
	if(!tweety.actorInstance) return;
	// TP: Set large z bias
	
	bmVectorCopy(tweety.destPosition, dest);
	tweety.destPosition[Y] = METERTOUNIT(2.5f);
	bmVectorCopy(tweety.poi, dest);

	drag = 0.01f;
	
	tweety.state = TWEETYSTATE_FOLLOW;
}

void SetTweetyAOI(ACTORINSTANCE *actor)
{
	tweety.aoi = actor;

	tweety.state = TWEETYSTATE_FOLLOW;
}

void TeleportTweety(TBVector position)
{
	bmVectorCopy(tweety.actorInstance->actorInstance.position, position);
}

void TweetyLaugh()
{
	FlushAnimationQueue(tweety.actorInstance);
	PlayAnimationByName(tweety.actorInstance,"laugh",1.0f,0,0,0.3f,NONE);
	PlayAnimationByName(tweety.actorInstance,"laugh",1.0f,0,0,0.3f,NONE);
	PlayAnimationByName(tweety.actorInstance,"laugh",1.0f,0,0,0.3f,NONE);
	tweety.state = TWEETYSTATE_SHORTLAUGH;
}

/* --------------------------------------------------------------------------------
   Function : DrawTweety
   Purpose : Draw tweety in front of all objects
   Parameters : 
   Returns : 
   Info : 
*/
static float bias = -100.0f;
void DrawTweety(void)
{
	if(!tweety.actorInstance) return;
	// TP: Set large z bias
	//SETZBIAS(bias);
	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();
	tweety.actorInstance->onScreen = baDrawActorInstance(&tweety.actorInstance->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
	//SETZBIAS(0.0f);
}

/* --------------------------------------------------------------------------------
   Function : UpdateTweetyLangauge
   Purpose : Update tweeties AI for the language select screen
   Parameters : time of update
   Returns : 
   Info : Tweetys flight pattern is tighter and faster
*/
TWEETY * UpdateTweetyLanguage(float fTime)
{
	TBVector	tempVector,distToTaz,tpos;
	static		int	index;
	float		tempFloat;

	if (!tweety.actorInstance) return NULL;

	bmVectorCopy(tpos,tweety.actorInstance->actorInstance.position);

	switch(tweety.state)
	{
	case TWEETYSTATE_FOLLOW:
				
		// TP: point to head to
		bmVectorSub(tempVector,tpos,tweety.poi);

		bmVectorSub(distToTaz,tweety.destPosition,tweety.actorInstance->actorInstance.position);

		if (bmVectorSquaredLen(distToTaz) < (METERTOUNIT(1.0f)*METERTOUNIT(1.0f)))
		{
			tweety.state = TWEETYSTATE_HOVER;
			accel = 5.0f;
			drag = 0.07f;
		}

		bmVectorCopy(tweety.destPosition,tweety.poi);

		tweety.destPosition[Y] = METERTOUNIT(2.0f);
		tweety.destPosition[Y] += METERTOUNIT(bmRandf() -0.5f);

		TweetyLookAt2D(tweety.poi);
		break;
	
	case TWEETYSTATE_HOVER:
		
		bmVectorSub(tempVector,tpos,tweety.poi);

		tempVector[Y] = 0.0f;

		bmVectorCopy(tweety.destPosition,tweety.poi);
		
		tweety.destPosition[Y] = METERTOUNIT(2.0f);
		tweety.destPosition[Y] += METERTOUNIT(bmRandf() -0.5f);

		TweetyLookAt2D(tweety.poi);

		if (bmVectorLen(tempVector) < METERTOUNIT(1.0f))
		{
			tweety.state = TWEETYSTATE_CIRCLE;
			if ((bmRand()%2)==0) tweety.rotDir = 0;
			else tweety.rotDir = 1;
		}

		break;
	
	case TWEETYSTATE_CIRCLE:
			
		bmVectorSub(tempVector,tpos,tweety.poi);

		tempVector[Y] = 0.0f;

		if ((tempFloat = bmVectorLen(tempVector)) > METERTOUNIT(1.1f))
			tweety.state = TWEETYSTATE_HOVER;

		if (tempFloat>0.0f)	bmVectorScaleToLength(tempVector,tempVector,METERTOUNIT(0.5f));

		bmVectorRotateY(tempVector,tempVector,tweety.rotDir==0?RAD(15.0f):RAD(-15.0f));

		if ((bmRand()%200)==0) tweety.rotDir = 1-tweety.rotDir;

		bmVectorAdd(tweety.destPosition,tempVector,tweety.poi);
		
		tweety.destPosition[Y] = METERTOUNIT(2.0f);
		tweety.destPosition[Y] += METERTOUNIT(bmRandf() -0.5f);

		TweetyLookAt2D(tweety.poi);

		break;

	case TWEETYSTATE_LAUGH:

		bmQuatCopy(tweety.actorInstance->actorInstance.orientation, bIdentityQuaternion);

		break;
	}

	MoveTweety();
	AnimateTweety();

	return &tweety;
}

/* --------------------------------------------------------------------------------
   Function : UpdateTweetyNormal
   Purpose : Update tweeties AI for al other levels
   Parameters : time of update
   Returns : 
   Info :  Tweetys flight pattern is much looser in this version
*/
int UpdateTweetyNormal(float fTime)
{
	TBVector	tempVector,distToTaz,tpos;
	static		int	index;
	float		tempFloat;

	if (!tweety.actorInstance) return 0;


	// TP: if (aoi) then work out point of interest
	if (tweety.aoi)
	{
		bmVectorCopy(tweety.poi,tweety.aoi->actorInstance.position);
	}

	bmVectorCopy(tpos,tweety.actorInstance->actorInstance.position);

/*	if (controller1.triangleChannel->value > 0.0f)
	{
		SetCameraLookAtFollowActorInstance(&cameraInfo,tweety.actorInstance, &tweety.actorInstance->actorInstance,(TBActorNodeInstance *)NULL);
	}
	else
	{
		SetCameraLookAtFollowActorInstance(&cameraInfo,gameStatus.player1, &gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
		tempVector[X] = 0;
		tempVector[Y] = METERTOUNIT(1);
		tempVector[Z] = 0;
		SetCameraLookAtActorInstanceOffset(&cameraInfo,tempVector);
	}
*/
	switch(tweety.state)
	{
	case TWEETYSTATE_FOLLOW:
		tweety.timeInState += fTime;
		
		// TP: point to head to
		bmVectorSub(tempVector,tpos,tweety.poi);

		/*if (bmVectorLen(tempVector) < METERTOUNIT(2.0f))
		{
			trailAdvanceToNext(gameStatus.player1, &index);
			trailGetIndexedPoint(gameStatus.player1, index, tweety.poi);
			tweety.poi[Y] += METERTOUNIT(1.0f);
			bmVectorCopy(tweety.destPosition,tweety.poi);
		}*/

		bmVectorSub(distToTaz,tweety.aoi->actorInstance.position,tweety.actorInstance->actorInstance.position);

		if (bmVectorSquaredLen(distToTaz) < (METERTOUNIT(2.0f)*METERTOUNIT(2.0f)))
		{
			tweety.timeInState = 0.0f;
			tweety.state = TWEETYSTATE_HOVER;
			//tweety.aoi = gameStatus.player1;
			accel = 4.0f;
			drag = 0.02f;
		}

		bmVectorCopy(tweety.destPosition,tweety.poi);

		TweetyLookAt2D(tweety.poi);
		break;
	case TWEETYSTATE_HOVER:
		tweety.timeInState += fTime;
		
		bmVectorSub(tempVector,tpos,tweety.poi);

		tempVector[Y] = 0.0f;

		bmVectorCopy(tweety.destPosition,tweety.poi);
		tweety.destPosition[Y] += METERTOUNIT(1.0f);

		TweetyLookAt2D(tweety.poi);

		if (bmVectorLen(tempVector) < METERTOUNIT(1.0f))
		{
			tweety.state = TWEETYSTATE_CIRCLE;
			if ((bmRand()%2)==0) tweety.rotDir = 0;
			else tweety.rotDir = 1;
			tweety.timeInState = 0.0f;
		}

		/*if (bmVectorLen(tempVector) > METERTOUNIT(3.0f))
		{
			index = trailFindNearestPoint(tweety.aoi, tweety.actorInstance->actorInstance.position, tweety.poi);

			if (index >= 0)
			{
				tweety.aoi = NULL;
				tweety.state = TWEETYSTATE_FOLLOW;
				tweety.timeInState = 0.0f;
				accel = 15.0f;
				drag = 0.1f;
			}
		}*/
		break;
	case TWEETYSTATE_CIRCLE:
		tweety.timeInState += fTime;
	
		bmVectorSub(tempVector,tpos,tweety.poi);

		tempVector[Y] = 0.0f;
		if ((tempFloat = bmVectorLen(tempVector)) > METERTOUNIT(1.1f))
		{
			tweety.state = TWEETYSTATE_HOVER;
			tweety.timeInState = 0.0f;
		}

		if (tempFloat>0.0f)	bmVectorScaleToLength(tempVector,tempVector,METERTOUNIT(1.0f));

		bmVectorRotateY(tempVector,tempVector,tweety.rotDir==0?RAD(15.0f):RAD(-15.0f));

		if ((bmRand()%200)==0) tweety.rotDir = 1-tweety.rotDir;

		bmVectorAdd(tweety.destPosition,tempVector,tweety.poi);
		tweety.destPosition[Y] += METERTOUNIT(1.0f);

		TweetyLookAt2D(tweety.poi);

		break;

	case TWEETYSTATE_SHORTLAUGH:
		break;
	}

	MoveTweety();
	AnimateTweety();

	if (tweety.state == TWEETYSTATE_FOLLOW)
		return 0;
	else
		return 1;
}

void DestroyTweety(void)
{
	if (tweety.actorInstance) 
	{
		FreeActorInstance(tweety.actorInstance);
		tweety.actorInstance = 0;
	}
}

bool interuptTweetyIntro(float time, float duration)
{
	if ((gameStatus.gameTime > (time + 1.0f)) && OKBUTTON)
		return true;

	if (gameStatus.gameTime > (time + duration))
		return true;

	return false;
}