// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : wantedposter.cpp
//   Purpose : functions to calculate the tilt of objects when collided with
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "collisioncache.h"
#include "control.h"
#include "scene.h"
#include "status.h"
#include "debris.h"
#include "main.h"
#include "animation.h"
#include "scene.h"
#include "trigger.h"
#include "playerstats.h"
#include "explode.h"
#include "holysmoke.h"
#include "wantedposter.h"
#include "thwap.h"

/* --------------------------------------------------------------------------------
   Function : CreateWantedPosterInstance
   Purpose : Create a destructibleObjectInstance
   Parameters : ACTORINSTANCE, type (DESTRUCTIBLETYPE_...)
   Returns : 
   Info : 
*/
int CreateWantedPosterInstance(ACTORINSTANCE *actorInstance, int32 posterNumber, int32 posterSpecialCase)
{
	actorInstance->special = SPECIALTYPE_WANTEDPOSTER;
	actorInstance->wantedPosterInfo = (WANTEDPOSTERINFO*)ZALLOC(sizeof(WANTEDPOSTERINFO));
	if (!actorInstance->destructibleObjectInfo) return FALSE;
	SetCollisionCallback(actorInstance,RespondToWantedPosterCollision);

	AddActorToCollisionCache(&collisionCache,actorInstance, 0);

	if (posterNumber==-1)
	{
		bkPrintf("      *** WARNING *** Poster number not specified.\n");
	}

	actorInstance->wantedPosterInfo->posterSpecialCase = posterSpecialCase;
	if(posterSpecialCase != -1)
	{
		switch(posterSpecialCase)
		{
		case POSTERSPECIALCASE_CLOCK:
			actorInstance->wantedPosterInfo->animationState = POSTERANIMSTATE_IDLE;
			break;
		}
	}

	actorInstance->wantedPosterInfo->posterNumber = posterNumber;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PreResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
int PreRespondToWantedPosterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible, PRECOLLINFO *info)
{
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToWantedPosterCollision
   Purpose : Function to be called when a collision with a wanted poster has occured
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *poster
   Returns : 
   Info : 
*/
int RespondToWantedPosterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *poster)
{
	TBVector pos,velocity,rotationSpeed;
	int i, player;
	char	message[256];

	if(poster->wantedPosterInfo->posterSpecialCase != -1) return FALSE;

	if (collider->actorStatus)
	{
		if(collider == gameStatus.player1) player = 1;
		else
			if(collider == gameStatus.player2) player = 2;
				else return FALSE;

		if (collider->actorStatus->currentState == STATE_SPIN)
		{
			bmVectorCopy(pos,poster->actorInstance.position);

			// send messages
			if (poster->wantedPosterInfo->onDestruction)
				SendTrigger(poster->wantedPosterInfo->onDestruction, pos);
			
			for (i=0;i<5;i++)
			{
				velocity[X] = (float)bmRand();
				velocity[Y] = (float)bmRand();
				velocity[Z] = (float)bmRand();
				velocity[X] -= 32768.0f/2.0f;
				velocity[Y] *= 2;
				velocity[Z] -= 32768.0f/2.0f;
				rotationSpeed[X] = (bmRand()%3)*PI;
				rotationSpeed[Y] = (bmRand()%5)*PI;
				rotationSpeed[Z] = 0.0f;
				rotationSpeed[W] = 1.0f;
				bmVectorScaleToLength(velocity,velocity,METERTOUNIT(4.8f));
				CreateDebrisActor(poster->actorName, pos, velocity, rotationSpeed,1.0f, 0.25f, 0, 0, 255, DEBRISFLAG_GRAVITY,0,0,0.0f);
			}
	
#ifndef STANDALONE
#ifdef CONSUMERDEMO
		if (demoGameTimer > 0.0f)
#endif
#endif
			// destroy destructible
			AddWantedPosterToStats(poster->wantedPosterInfo->posterNumber, collider, map.sceneNumber);  //gameStatus.currentScene);

			//bmVectorCopy(pos,poster->actorInstance.position);

			// send messages
			//sprintf(message,"_POS=%f %f %f",pos[X],pos[Y],pos[Z]);
			//bkGenerateEvent("sound",message);
			//if (poster->wantedPosterInfo->onDestruction)
			//	SendTrigger(poster->wantedPosterInfo->onDestruction);

			RemoveActorFromCollisionCache(&collisionCache,poster);
			FreeActorInstance(poster);
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateSpecialCaseWantedPoster
   Purpose : update function for wanted posters that require individual attention, hence special case
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/

int UpdateSpecialCaseWantedPoster(ACTORINSTANCE *ptr)
{
	if(ptr->wantedPosterInfo)
	{
		if(ptr->wantedPosterInfo->posterSpecialCase != -1)
		{
			switch(ptr->wantedPosterInfo->posterSpecialCase)
			{
			case POSTERSPECIALCASE_CLOCK:
				switch(ptr->wantedPosterInfo->animationState)
				{
				case POSTERANIMSTATE_IDLE:
					if(!CheckIfPlayingAnimation(ptr, "tick"))
					{
						PlayAnimationByName(ptr, "tick", 1.0f, 1, 0, 0.1f, IDLE);
					}
					// CMD: 
					//------------------------------------
					//temp trigger event
/*						if(controller1.r2DebounceChannel->value)
					{
						PlayAnimationByName(ptr, "windup", 1.0f, 0, 0,0.1f, JUMP);
						ptr->wantedPosterInfo->animationState = POSTERANIMSTATE_EVENT;
					}*/
					//------------------------------------
					break;
				case POSTERANIMSTATE_EVENT:
					if(!CheckIfPlayingAnimation(ptr, "windup"))
					{
						PlayAnimationByName(ptr, "xplode", 0.25f, 0, 0, 0.1f, DEATH);
						StartExplosion(ptr->actorInstance.position, 0.1f, 200.0f, "explosion.wav", EXPLOSIONTYPE_CARTOON);
						// TP: PROBLEM
						camera[0].quakeOffsetLevel = 0.1f;
						camera[0].tiltOffsetLevel = RAD(5.0f);
						camera[0].flags |= CAMERA_QUAKEFADE;
						ptr->wantedPosterInfo->animationState = POSTERANIMSTATE_SPENT;
					}
					break;
				case POSTERANIMSTATE_SPENT:
					if(CheckIfPlayingAnimation(ptr, "xplode"))
					{
						DrawSmokeAtNodeInstance(ptr,"clockdinger1", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockdinger2", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockface01", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockface02", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockface03", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockface04", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockpendulum", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockpin", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockhandhour", 0, SMOKE_EXPLOSION);
						DrawSmokeAtNodeInstance(ptr,"clockhandminute", 0, SMOKE_EXPLOSION);
					}
					else
					{
						RefreshCollisionEntry(&collisionCache,ptr);
						ptr->wantedPosterInfo->animationState = POSTERANIMSTATE_DEAD;
					}
					break;
				case POSTERANIMSTATE_DEAD:
					break;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}