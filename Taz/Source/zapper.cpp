// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : zapper.cpp
//   Purpose : functions to calculate the tilt of objects when collided with
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "characters.h"
#include "costumes.h"
#include "status.h"
#include "control.h"
#include "animation.h"
#include "zapper.h"

/* --------------------------------------------------------------------------------
   Function : CreateZapper
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateZapper(ACTORINSTANCE *actorInstance, EZapperType type)
{
	actorInstance->special = SPECIALTYPE_ZAPPER;

	actorInstance->zapperInfo = (ZAPPERINFO*)ZALLOC(sizeof(ZAPPERINFO));
	ASSERTM(actorInstance->zapperInfo, "no memory left for zappers\n");

	actorInstance->zapperInfo->type = type;
	actorInstance->zapperInfo->effectClock = 0.0f;
	actorInstance->zapperInfo->flags = 0;

	//set up lightning info
	actorInstance->zapperInfo->lightningInfo.calcStartPoint = FALSE;
	actorInstance->zapperInfo->lightningInfo.state = FALSE;
	actorInstance->zapperInfo->lightningInfo.size[0] = METERTOUNIT(0.1f);
	actorInstance->zapperInfo->lightningInfo.texture = bkLoadTexture(NULL,"green.tga",0);
	bmVectorCopy(actorInstance->zapperInfo->lightningInfo.currentStartPos, actorInstance->actorInstance.position);
	return TRUE;
}

void MouseStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, MOVE);
	actorInstance->actorAnimation->useMotionBone = TRUE;
}

void BallStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	actorInstance->actorInstance.position[1] += METERTOUNIT(0.3895f);
	actorInstance->actorBehaviour->collSphere.offset[1] = 0.0f;
	actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.3895f);
	actorInstance->actorAnimation->useMotionBone = FALSE;
	SetActorMinFricAngle(actorInstance->actorBehaviour, 1.0f);
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	worldGravity = NORMAL_WORLD_GRAVITY*0.25f;
}

void TazToMouseCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 0, 0.1f, REACT);
	actorInstance->actorAnimation->useMotionBone = TRUE;
}

void BallToMouseCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, MOVE);
	actorInstance->actorAnimation->useMotionBone = TRUE;
}

void MouseToBallCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	actorInstance->actorInstance.position[1] += METERTOUNIT(0.3895f);
	actorInstance->actorBehaviour->collSphere.offset[1] = 0.0f;
	actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.3895f);
	actorInstance->actorAnimation->useMotionBone = FALSE;
	SetActorMinFricAngle(actorInstance->actorBehaviour, 1.0f);
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	worldGravity = NORMAL_WORLD_GRAVITY*0.25f;
}

void MouseToTazCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, MOVE);
	actorInstance->actorAnimation->useMotionBone = TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateZapper
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateZapper(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE	*taz = gameStatus.player[0].actorInstance;

	if(actorInstance->zapperInfo)
	{

		// CMD: only draw lightning if clock says to
		if((actorInstance->zapperInfo->effectClock -= fTime) > 0.0f)
		{
			if(!actorInstance->zapperInfo->lightningInfo.state)
			{
				actorInstance->zapperInfo->lightningInfo.state = TRUE;
			}
			actorInstance->zapperInfo->lightningInfo.calcStartPoint = FALSE;
			if(CreateLightningNodePoints(&actorInstance->zapperInfo->lightningInfo, taz))
			{
				actorInstance->zapperInfo->lightningInfo.state = TRUE;
			}
			// CMD: hold taz still in a very crude manner
			bmVectorSet(gameStatus.player[0].actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			actorInstance->zapperInfo->lightningInfo.clock = 0.0f;
			actorInstance->zapperInfo->lightningInfo.state = FALSE;
			actorInstance->zapperInfo->lightningInfo.calcStartPoint = TRUE;
		}

		if(actorInstance->zapperInfo->flags & ZAPPER_ACTIVE)
		{
			// CMD: zapper active, but effect clock wound down, so change taz to frog
			// CMD: and cover it up with explosion
//				if(taz->actorStatus->currentState == STATE_SPIN)
//				{
//					SpinDown(&gameStatus.player[0]);
//				}
			switch(actorInstance->zapperInfo->type)
			{
			case ZAPPER_MOUSE:
				if((taz->actorStatus->currentState != STATE_MOUSE)&&(taz->actorStatus->currentState != STATE_ZAPPEDINTOMOUSE))
				{
					switch(taz->actorStatus->currentState)
					{
					case STATE_BALL:
						RequestStateChangePlus(taz, STATE_ZAPPEDINTOMOUSE, BallToMouseCallback);
						break;
					default:
						RequestStateChangePlus(taz, STATE_ZAPPEDINTOMOUSE, TazToMouseCallback);
						break;
					}
				}
				actorInstance->zapperInfo->flags &= ~ZAPPER_ACTIVE;
				break;
			case ZAPPER_BALL:
				if((taz->actorStatus->currentState != STATE_BALL)&&(taz->actorStatus->currentState != STATE_ZAPPEDINTOBALL))
				{
					// CMD: can only go to ball from mouse
					switch(taz->actorStatus->currentState)
					{
					case STATE_MOUSE:
						RequestStateChangePlus(taz, STATE_ZAPPEDINTOBALL, MouseToBallCallback);
						break;
					}
				}
				actorInstance->zapperInfo->flags &= ~ZAPPER_ACTIVE;
				break;
			case ZAPPER_TAZ:
				if((taz->actorStatus->currentState == STATE_MOUSE)&&(taz->actorStatus->currentState != STATE_ZAPPEDINTOTAZ))
				{
					RequestStateChangePlus(taz, STATE_ZAPPEDINTOTAZ, MouseToTazCallback);
				}
				actorInstance->zapperInfo->flags &= ~ZAPPER_ACTIVE;
				break;
			}
		}
	}
}