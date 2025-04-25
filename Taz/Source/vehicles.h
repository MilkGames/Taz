#ifndef __H_VEHICLES
#define __H_VEHICLES

#include "trolley.h"
#include "floorpolisher.h"
#include "hovercraft.h"
#include "rocketbike.h"
#include "animation.h"
#include "minecart.h"

#define	MAX_TROLLEY_TURN	(RAD(180.0f))
#define MAX_TROLLEY_SPEED	(METERTOUNIT(12.0f))

enum
{
	VEHICLE_TROLLEY,
	VEHICLE_POLISHER,
	VEHICLE_HOVERCRAFT,
	VEHICLE_ROCKETBIKE,
	VEHICLE_MINECART,
};

enum
{
	VEHICLE_STATE_OFF,
	VEHICLE_STATE_ON,
	VEHICLE_STATE_NOESCAPE,
};

// info struct held by vehicle
typedef struct TAG_VEHICLEINFO
{
	TBVector					orientationVector;
								
	TBVector					tazOffset;
								
	int32						type;
	int32						state;
	int							handle;
	struct TAG_ACTORINSTANCE	*vehicleInstance;			//ptr to original vehicle object

	float						controllerInputTimer;		//value to limit the frequency at which pad input is sampled
	float						currentRotation;
	float						currentRotationalVelocity;
	int							soundHandle;

	// PP: data specific to particular vehicle types...
	union
	{	
		POLISHERINFO			*polisherInfo;				// PP: data specific to the POLISHER vehicle type

		// TP: got compiler warning about deleting this void *, I assume it's not sure what destructors to call
		// TP: void					*typeSpecificData;			// PP: dummy pointer representing all the different type-specific pointers
	};

	ANIMBLENDINFO	animBlendInfo;
	float			clock;
	float			rate;
	uint32			numSoFar;
	float			timeSinceCameraColl;
	int32			iPad;

} VEHICLEINFO, _VEHICLEINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateVehicle
   Purpose : Function to control the creation of vehicles
   Parameters : 
   Returns : 
   Info : 
*/

int CreateVehicle(struct TAG_ACTORINSTANCE *vehicle, int32 type);

/* --------------------------------------------------------------------------------
   Function : CreateVehicleInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VEHICLEINFO *CreateVehicleInfo(void);

/* --------------------------------------------------------------------------------
   Function : PlaceVehicle
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int PlaceVehicle(ACTORINSTANCE *vehicle, TBVector pos);


/* --------------------------------------------------------------------------------
   Function : FindVehicle
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ACTORINSTANCE *FindVehicle(int32 type, int which = 0);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateVehicles
	Purpose 	: update any special stuff to do with vehicles, eg. floor polisher trail
	Parameters 	: ptr to actor instance list from which to update vehicles
	Returns 	: 
	Info 		: // PP: 
*/
void UpdateVehicles(struct TAG_ACTORINSTANCELIST* const actorInstanceList);

/*	--------------------------------------------------------------------------------
	Function 	: DrawVehicles
	Purpose 	: draw any special stuff to do with vehicles, eg. floor polisher trail
	Parameters 	: ptr to actor instance list from which to draw vehicles
	Returns 	: 
	Info 		: // PP: 
*/
void DrawVehicles(struct TAG_ACTORINSTANCELIST* const actorInstanceList);


#endif