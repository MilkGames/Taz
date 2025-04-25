// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : hiccup.cpp
//   Purpose : functions to update and draw hiccup effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "status.h"
#include "maths.h"
#include "physics.h"
#include "animation.h"
#include "sfx.h"
#include "characters.h"
#include "costumes.h"
#include "hiccup.h"
#include "mapfile.h"
#include "scene.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateActorHiccup(ACTORINSTANCE *actorInstance)
{
	TBVector		tvec;
	TBQuaternion	tempQuat;
	TBActorAnimSegment	*animation;

	if(((actorInstance->characterInfo->powerUpInfo.clock += fTime) > POWERUPTIME_HICCUP)&&(map.sceneNumber != SCENE_DESTRUCTIONTASMANIA))
	{
		FinishHiccup(actorInstance);
		return;
	}

	if((actorInstance->actorStatus->currentState != STATE_JUMP)&&(actorInstance->actorStatus->currentState != STATE_SPINUP))
	{
		if((actorInstance->characterInfo->powerUpInfo.randomClock -= fTime) < 0.0f)
		{
			if(actorInstance->actorStatus->currentState == STATE_SPIN)
			{
				// reset behaviour
				SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				if(actorInstance->characterInfo->spinInfo)
				{
					actorInstance->characterInfo->spinInfo->spinUpdated = FALSE;
				}

				actorInstance->actorStatus->targetSpinSpeed = 0.0f;

				FlushAnimationQueue(actorInstance);
				bmQuatCopy(tempQuat,actorInstance->actorInstance.orientation);
				ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL);
				if(strcmpi(actorInstance->actorName, "shedevil.obe") == 0)// CPW: stinky bodge
					AttachObjectToCharacter(actorInstance,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

				EnableActorCartoonRenderMode(&actorInstance->actorInstance);
				bmQuatCopy(actorInstance->actorInstance.orientation,tempQuat);
				ResetCharacterAfterJump(actorInstance);
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
				actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;

				StopChannel(&actorInstance->actorStatus->soundHandle[0]);
				actorInstance->actorStatus->soundHandle[0] = PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
				StopChannel(&actorInstance->actorStatus->soundHandle[1]);
				
				// PP: Setup costume even if none is being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
				SetupCostume(actorInstance,actorInstance->characterInfo->costume);

				//set hiccup start anim
				animation = baFindAnimSegment(&actorInstance->actorInstance,"hicuprise");
				baPrepActorInstance(&actorInstance->actorInstance, TRUE);
			}
			else
			{
				animation = baFindAnimSegment(&actorInstance->actorInstance,"hicupstart");
			}

			actorInstance->characterInfo->powerUpInfo.randomClock = (float)(bmRand()%25)/10.0f;

			CalculateSpeedBeforeJump(actorInstance);							// fill in current speed before jump
			FlushAnimationQueue(actorInstance);
			PlayAnimationBySegmentPointer(actorInstance,animation,1.0f,0,0,0.1f,HICCUP);	// If I could play jump then
			PlayAnimationByName(actorInstance,"hicuprise",0.3f,0,1,0.1f,HICCUP);
			actorInstance->characterInfo->powerUpInfo.hiccupJumpState = true;
			actorInstance->actorInstance.position[Y] += 1.0f;					// bodge to avoid sticking to ground

			// zero vertical speed before adding jump
			actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
			SETVECTOR(tvec,0.0f,1.0f,0.0f,1.0f);
			AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(15.0f));//METERTOUNIT(6.324f)
			RequestBuzzerEffect(gameStatus.player[actorInstance->playerNo].controller, 0.5f, 0.0f, WAVEFORM_RANDOM);

			// Queue fall animation
			PlayAnimationByName(actorInstance,"hicupfall",0.4f,1,1,0.4f,HICCUP);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = FALSE;

			// keep track of position prior to jump move
			bmVectorCopy(actorInstance->actorStatus->positionBeforeJump,actorInstance->actorInstance.position);

			PlaySample("hiccup.wav",255,actorInstance->actorInstance.position);
			RequestStateChange(actorInstance,STATE_JUMP);
//			actorInstance->actorStatus->currentState = STATE_JUMP;
			//-----------------STATE CHANGE-----------------
			return;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FinishHiccup
	Purpose		: End the hiccup powerup
	Parameters	: actorinstance
	Returns		: 
	Info		: 
*/
void FinishHiccup(ACTORINSTANCE *actorInstance)
{
	actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
	actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
	actorInstance->characterInfo->powerUpInfo.hiccup = FALSE;
	actorInstance->characterInfo->powerUpInfo.hiccupJumpState = false;
}
