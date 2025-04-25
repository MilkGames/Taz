// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : pneumaticplunger.cpp
//   Purpose : functions required to model a neumatic plunger
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h" 

#include "global.h"

#include "actors.h"
#include "scene.h"
#include "attach.h"
#include "status.h"
#include "pneumaticplunger.h"

CURRENTPLUNGERINFO		currentPlungerInfo;

/* --------------------------------------------------------------------------------
   Function : CreatePneumaticPlunger
   Purpose : Creates and initialises actorInstances that act as pneumaticplungers
   Parameters : 
   Returns : 
   Info : 
*/

void CreatePneumaticPlunger(TBVector position)
{
	/*
	ACTORINSTANCE *plunger;

	plunger = CreateActorInstance(&levelInstances, "mailbox.obe", "theKaraokeKing");
	CreateActorInstanceBehaviour(plunger);
	CreatePneumaticPlungerInfo(plunger);

	EnableActorCartoonRenderMode(&plunger->actorInstance);

	//set attributes
	plunger->special = SPECIALTYPE_PNEUMATICPLUNGER;
	bmVectorCopy(plunger->pneumaticPlungerInfo->initialPosition, position);
	plunger->pneumaticPlungerInfo->onPlunger = FALSE;
	
	SetActorPosition(plunger, position);

	AddActorToCollisionCache(&collisionCache, plunger, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(plunger, RespondToPneumaticPlungerCollision);
	*/
}

/* --------------------------------------------------------------------------------
   Function : CreatePneumeticPlungerInfo
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int CreatePneumaticPlungerInfo(ACTORINSTANCE *actorInstance)
{
	/*
	actorInstance->pneumaticPlungerInfo = (PNEUMATICPLUNGERINFO*)MALLOC(sizeof(PNEUMATICPLUNGERINFO));
	if (actorInstance->pneumaticPlungerInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create pneumatic plunger info structure\n");
		return FALSE;
	}
	*/
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdatePneumeticPlunger
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void UpdatePneumaticPlunger(ACTORINSTANCE *plunger)
{
}

/* --------------------------------------------------------------------------------
   Function : RespondToPneumaticPlungerCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToPneumaticPlungerCollision(ACTORINSTANCE *collider, ACTORINSTANCE *plunger)
{
/*	TBVector		relativePos;
	TBVector		relativeScale;
	TBVector		relativeQuat;

	if(collider->actorStatus->currentState == STATE_MOVE)
	{
		relativePos[0] = 0.0f; relativePos[1] = 0.0f; relativePos[2] = 0.0f; relativePos[3] = 0.0f;
		relativeScale[0] = 1.0f; relativeScale[1] = 1.0f; relativeScale[2] = 1.0f; relativeScale[3] = 1.0f;
		bmQuatCopy(relativeQuat, bIdentityQuaternion);

		plunger->pneumaticPlungerInfo->onPlunger = TRUE;
		RequestStateChange(collider, STATE_ONPNEUMATICPLUNGER);
		currentPlungerInfo.handle = AttachObjectToCharacter(collider, "trolley.obe", "root", relativePos, relativeScale, relativeQuat, 0);
		bmVectorCopy(currentPlungerInfo.homePosition, plunger->pneumaticPlungerInfo->initialPosition);
		FreeActorInstance(plunger);
	}

	return TRUE;						//requre physics to be processed
	*/
	return TRUE;
}