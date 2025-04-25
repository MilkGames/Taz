#ifndef __H_AIRCON
#define __H_AIRCON

#include "actors.h"

#define AIRCON_PERIOD		(0.125f)
#define AIRCON_MAX_SPEED	(METERTOUNIT(2.0f))
#define AIRCON_THRESHOLD	(METERTOUNIT(30.0f))
#define MAX_AIRCON			(36)

typedef struct TAG_AIRCONUNITINFO
{
	TBVector		direction;

	float			clock;
	int32			iPad[3];
} AIRCONUNITINFO, _AIRCONUNITINFO BALIGN16;

typedef struct TAG_AIRCONINFO
{
	TBVector		position;
	TBVector		velocity;

	float			alpha;
	float			size;
	float			angle;
	float			rotSpeed;

	uint32			alphaUp;
	int32			iPad[3];
} AIRCONINFO, _AIRCONINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateAirconUnit
   Purpose : Creates an air conditioning unit at the specified point, in the specified orientation
   Parameters : 
   Returns : 
   Info : 
*/

void CreateAirconUnit(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateAirconUnit
   Purpose : Creates an air conditioning unit at the specified point, in the specified orientation
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateAirconUnit(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitAirconExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitAirconExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : AirconExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : AirconExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/

int AirconExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : AirconExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : AirconExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : AirconExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void AirconExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif