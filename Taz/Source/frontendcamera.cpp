// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : frontendmain.cpp
//   Purpose : functions to control the frontend
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "maths.h"
#include "frontendmain.h"

FECAMSTORE frontEndCamStore[MENU_NUMSUBMENUS];

/*	--------------------------------------------------------------------------------
	Function 	: StoreFrontEndCameraInfoFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseFrontEndCameraStore(void)
{
	for(int i=FECAM_MAIN;i<FECAM_NUMOFCAMERAS;i++)
	{
		frontEndCamStore[i].actorInstance = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StoreFrontEndCameraInfoFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StoreFrontEndCameraInfoFromMap(ACTORINSTANCE *actorInstance, int32 position, uint32 lookAtInstance)
{
	if(position != -1)
	{
		frontEndCamStore[position].actorInstance = actorInstance;
		frontEndCamStore[position].position = position;
		frontEndCamStore[position].lookAtInstanceCRC = lookAtInstance;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AcceptFrontEndCamerasFromMap
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void AcceptFrontEndCamerasFromStore(ACTORINSTANCE *actorInstance, int32 position, uint32 lookAtInstance)
{
	ACTORINSTANCE	*temp;

	if(actorInstance)
	{
		bmVectorCopy(frontEndCameraInfo.cameras[position][FECAMPOS], actorInstance->actorInstance.position);
		//find look at instance in invisible instance list
		temp = FindActorInstanceInInstanceList(NULL,lookAtInstance,&map.invisibleInstances);
		if(temp)
		{
			bmVectorCopy(frontEndCameraInfo.cameras[position][FELOOKATPOS],temp->actorInstance.position);
		}
		else
		{
			bkPrintf("*** WARNING *** could not find front end camera look at instance.\n");
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeFrontEndCameraStore
	Purpose 	: frees memory used by front end camera store
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEndCameraStore(void)
{
	for(int i=FECAM_MAIN;i<FECAM_NUMOFCAMERAS;i++)
	{
		if(frontEndCamStore[i].actorInstance)
		{
			FreeActorInstanceMemory(frontEndCamStore[i].actorInstance,0);
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SetFrontEndCameraDestination
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void SetFrontEndCameraDestination(ECameras currentCamera, ECameras destinationCamera)
{
	ACTORINSTANCE	*tempInstance;
	TBVector		temp[2];

	frontEndCameraInfo.time = 0.0f;
	bmVectorCopy(frontEndCameraInfo.tempCamStart[FECAMPOS], frontEndCameraInfo.cameraInfo->pos);
	bmVectorCopy(frontEndCameraInfo.tempCamStart[FELOOKATPOS], frontEndCameraInfo.cameraInfo->lookAt);
	
	switch(currentCamera)
	{
	case FECAM_MAIN:
		switch(destinationCamera)
		{
		case FECAM_STARTGAME:
			tempInstance = FindActorInstanceInInstanceList("starttosaveone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttosavetwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		case FECAM_OPTIONS:
			tempInstance = FindActorInstanceInInstanceList("starttooptionsone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttooptionstwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		case FECAM_AUDIO:
			tempInstance = FindActorInstanceInInstanceList("starttoaudioone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttoaudiotwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		case FECAM_TWOPLAYER:
			tempInstance = FindActorInstanceInInstanceList("starttomultiplayerone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttomultiplayertwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		case FECAM_RECORDS:
			tempInstance = FindActorInstanceInInstanceList("starttorecordsone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttorecordstwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		case FECAM_EXTRAS:
			tempInstance = FindActorInstanceInInstanceList("starttoextrasone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("starttoextrastwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
			break;
		}
		break;
	case FECAM_STARTGAME:
		tempInstance = FindActorInstanceInInstanceList("starttosavetwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("starttosaveone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_OPTIONS:
		tempInstance = FindActorInstanceInInstanceList("starttooptionstwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("starttooptionsone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_AUDIO:
		tempInstance = FindActorInstanceInInstanceList("starttoaudiotwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("starttoaudioone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_TWOPLAYER:
		switch(destinationCamera)
		{
		case FECAM_TWOPLAYER_TOURNAMENT:
			tempInstance = FindActorInstanceInInstanceList("twoplayertotournamentone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("twoplayertotournamenttwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		case FECAM_TWOPLAYER_TIME:
			tempInstance = FindActorInstanceInInstanceList("twoplayertotimeone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("twoplayertotimetwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		case FECAM_TWOPLAYER_DESTRUCTION:
			tempInstance = FindActorInstanceInInstanceList("twoplayertodestructionone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("twoplayertodestructiontwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		case FECAM_TWOPLAYER_RACE:
			tempInstance = FindActorInstanceInInstanceList("twoplayertoraceone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("twoplayertoracetwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		default:
			bmVectorCopy(temp[0],frontEndCameraInfo.controlPts[0]);
			bmVectorCopy(temp[1],frontEndCameraInfo.controlPts[1]);

			bmVectorCopy(frontEndCameraInfo.controlPts[0],temp[1]);
			bmVectorCopy(frontEndCameraInfo.controlPts[1],temp[0]);
			break;
		}
		break;
	case FECAM_TWOPLAYER_TOURNAMENT:
		tempInstance = FindActorInstanceInInstanceList("twoplayertotournamenttwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("twoplayertotournamentone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_TWOPLAYER_TIME:
		tempInstance = FindActorInstanceInInstanceList("twoplayertotimetwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("twoplayertotimeone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_TWOPLAYER_DESTRUCTION:
		tempInstance = FindActorInstanceInInstanceList("twoplayertodestructiontwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("twoplayertodestructionone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_TWOPLAYER_RACE:
		tempInstance = FindActorInstanceInInstanceList("twoplayertoracetwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("twoplayertoraceone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_RECORDS:
		tempInstance = FindActorInstanceInInstanceList("starttorecordstwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("starttorecordsone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;

	case FECAM_EXTRAS:
//		tempInstance = FindActorInstanceInInstanceList("starttoextrastwo",0,&map.invisibleInstances);
//		if(!tempInstance) return;
//		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
//		tempInstance = FindActorInstanceInInstanceList("starttoextrasone",0,&map.invisibleInstances);
//		if(!tempInstance) return;
//		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
//		break;
		
		switch(destinationCamera)
		{
		case FECAM_EXTRAS_CREDITS:
			tempInstance = FindActorInstanceInInstanceList("extrastocreditsone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			// NH: Note 'credit' NO 'S' on this one!!! May get Bucky to fix, but not v important
			tempInstance = FindActorInstanceInInstanceList("extrastocredittwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		case FECAM_EXTRAS_GALLERY:
			tempInstance = FindActorInstanceInInstanceList("extrastogalleryone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("extrastogallerytwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		case FECAM_EXTRAS_SECRETS:
			tempInstance = FindActorInstanceInInstanceList("extrastosecretsone",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
			tempInstance = FindActorInstanceInInstanceList("extrastosecretstwo",0,&map.invisibleInstances);
			if(!tempInstance) return;
			bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);		
			break;
		default:
			bmVectorCopy(temp[0],frontEndCameraInfo.controlPts[0]);
			bmVectorCopy(temp[1],frontEndCameraInfo.controlPts[1]);

			bmVectorCopy(frontEndCameraInfo.controlPts[0],temp[1]);
			bmVectorCopy(frontEndCameraInfo.controlPts[1],temp[0]);
			break;		
		}
		break;
	case FECAM_EXTRAS_CREDITS:
		tempInstance = FindActorInstanceInInstanceList("extrastocredittwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("extrastocreditsone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_EXTRAS_GALLERY:
		tempInstance = FindActorInstanceInInstanceList("extrastogallerytwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("extrastogalleryone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	case FECAM_EXTRAS_SECRETS:
		tempInstance = FindActorInstanceInInstanceList("extrastosecretstwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("extrastosecretsone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	/*
		tempInstance = FindActorInstanceInInstanceList("starttoextrastwo",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[0], tempInstance->actorInstance.position);
		tempInstance = FindActorInstanceInInstanceList("starttoextrasone",0,&map.invisibleInstances);
		if(!tempInstance) return;
		bmVectorCopy(frontEndCameraInfo.controlPts[1], tempInstance->actorInstance.position);
		break;
	*/
	}
	frontEndCameraInfo.destinationMenu = destinationCamera;
	frontEndCameraInfo.currentMenu = currentCamera;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndCamera
	Purpose 	: Reads camera positions from map
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndCamera(void)
{
	TBVector	tempVector;

	bmVectorSub(tempVector,frontEndCameraInfo.cameraInfo->pos, frontEndCameraInfo.cameras[frontEndCameraInfo.destinationMenu][FECAMPOS]);
	if(bmVectorLen(tempVector) < METERTOUNIT(0.05f))
	{
		bmVectorCopy(frontEndCameraInfo.cameraInfo->lookAt,frontEndCameraInfo.cameras[frontEndCameraInfo.destinationMenu][FELOOKATPOS]);
		bmVectorCopy(frontEndCameraInfo.cameraInfo->pos, frontEndCameraInfo.cameras[frontEndCameraInfo.destinationMenu][FECAMPOS]);
		frontEndCameraInfo.currentMenu = frontEndCameraInfo.destinationMenu;
		frontEndCameraInfo.time = 0.0f;
	}
	else
	{
		FindPointOnCurve(frontEndCameraInfo.cameraInfo->destinationPos,frontEndCameraInfo.tempCamStart[FECAMPOS],
			frontEndCameraInfo.controlPts[0],frontEndCameraInfo.controlPts[1],
			frontEndCameraInfo.cameras[frontEndCameraInfo.destinationMenu][FECAMPOS],frontEndCameraInfo.time);

		FindPointOnCurve(frontEndCameraInfo.cameraInfo->destinationLookAt,frontEndCameraInfo.tempCamStart[FELOOKATPOS],
			frontEndCameraInfo.controlPts[0],frontEndCameraInfo.controlPts[1],
			frontEndCameraInfo.cameras[frontEndCameraInfo.destinationMenu][FELOOKATPOS],frontEndCameraInfo.time);
	}
}
