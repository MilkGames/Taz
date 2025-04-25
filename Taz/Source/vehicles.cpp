// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : vehicles.cpp
//   Purpose : generic vehicle functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "vehicles.h"

// PP:  actor instance used to z-mask floor-polisher trails in the Museum Vehicle Race
TBActorInstance*		MuseumVehicleRace_floorPolisherTrailZMask;// PP: gogogogh


/* --------------------------------------------------------------------------------
   Function : CreateVehicle
   Purpose : Function to control the creation of vehicles
   Parameters : 
   Returns : 
   Info : 
*/

int CreateVehicle(ACTORINSTANCE *vehicle, int32 type)
{
	if((vehicle->vehicleInfo = CreateVehicleInfo()) == NULL) return FALSE;

	vehicle->special = SPECIALTYPE_VEHICLE;
	vehicle->vehicleInfo->type = type;
	vehicle->vehicleInfo->vehicleInstance = vehicle;
	vehicle->vehicleInfo->controllerInputTimer = 0.0f;
	vehicle->vehicleInfo->currentRotation = 0.0f;
	vehicle->vehicleInfo->currentRotationalVelocity = 0.0f;
	vehicle->vehicleInfo->soundHandle = -1;
	vehicle->vehicleInfo->animBlendInfo.currentBlend = 0.0f;
	vehicle->vehicleInfo->animBlendInfo.progress = 0.0f;
	vehicle->vehicleInfo->animBlendInfo.smooth = 0.15f;
	vehicle->vehicleInfo->state=VEHICLE_STATE_OFF;				// PP: vehicle starts in 'off' state

	vehicle->vehicleInfo->polisherInfo=NULL;

	switch(type)
	{
	case VEHICLE_TROLLEY:
		CreateTrolley(vehicle);
		break;
#ifndef CONSUMERDEMO
	case VEHICLE_POLISHER:
		CreateFloorPolisher(vehicle);
		break;
	case VEHICLE_HOVERCRAFT:
		CreateHovercraft(vehicle);
		break;
	case VEHICLE_ROCKETBIKE:
		CreateRocketbike(vehicle);
		break;
#endif
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateVehicleInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VEHICLEINFO *CreateVehicleInfo(void)
{
	VEHICLEINFO		*tempStruct;

	tempStruct = (VEHICLEINFO*)ZALLOC(sizeof(VEHICLEINFO));

	if(tempStruct)
	{
		return tempStruct;
	}
	else
	{
		bkPrintf("vehicle info structure could not be created\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : PlaceVehicle
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int PlaceVehicle(ACTORINSTANCE *vehicle, TBVector pos)
{
	bmVectorCopy(vehicle->actorInstance.position, pos);

	return 0;
}

/* --------------------------------------------------------------------------------
   Function : FindVehicle
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ACTORINSTANCE *FindVehicle(int32 type, int which)
{
	ACTORINSTANCE *p;

	for(p = map.levelInstances.head.next; p != &map.levelInstances.head; p = p->next)
	{
		if(p->special == SPECIALTYPE_VEHICLE)
		{
			if(p->vehicleInfo->type == type)
			{
				if(which == 0)
					return p;
				else
					which--;
			}
		}
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateVehicles
	Purpose 	: update any special stuff to do with vehicles, eg. floor polisher trail
	Parameters 	: ptr to actor instance list from which to update vehicles
	Returns 	: 
	Info 		: // PP: 
*/
void UpdateVehicles(struct TAG_ACTORINSTANCELIST* const actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	bool			testVisibility;		// PP: should the visibility status of each vehicle be fully re-tested at this stage?

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->special == SPECIALTYPE_VEHICLE)
		{
			if(ptr->vehicleInfo != NULL)
			{
#ifndef CONSUMERDEMO
				switch(ptr->vehicleInfo->type)
				{
				case VEHICLE_POLISHER:
					{
						if((ptr->vehicleInfo->state == VEHICLE_STATE_ON)// PP: always update a vehicle that's being ridden
							||ptr->vehicleInfo->polisherInfo->shouldUpdate())
						{
							UpdateFloorPolisher(ptr);
						}
					}
					break;
				}
#endif
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: DrawVehicles
	Purpose 	: draw any special stuff to do with vehicles, eg. floor polisher trail
	Parameters 	: ptr to actor instance list from which to draw vehicles
	Returns 	: 
	Info 		: // PP: 
*/
void DrawVehicles(struct TAG_ACTORINSTANCELIST* const actorInstanceList)
{
	ACTORINSTANCE *ptr;

#ifndef CONSUMERDEMO
	// PP: apply a z mask object to our polisher trails - this masks off carpets and the like, which wouldn't *reeeely* go all shiny now would they.  No.  Well stop your silly nonsense then.
	if(POLISHERINFO::getTrailZMask() != NULL)
	{
		ApplyDecalZMask(POLISHERINFO::getTrailZMask(), POLISHERINFO::getMaxTrailYOffset());
	}
#endif

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->special == SPECIALTYPE_VEHICLE)
		{
			if(ptr->vehicleInfo != NULL)
			{
#ifndef CONSUMERDEMO
				switch(ptr->vehicleInfo->type)
				{
				case VEHICLE_POLISHER:
					{
						if(ptr->vehicleInfo->polisherInfo->shouldDraw())
						{
							DrawFloorPolisher(ptr);
						}
					}
					break;
				}
#endif
			}
		}
	}
}
