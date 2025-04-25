#ifndef __H_SPARKS
#define __H_SPARKS

#include "ParseMap.h"

#define MAX_CRAZYSPARKS		(120)
#define CRAZYSPARKS_SPEED	(1000)						// CMD: in units per second
#define CRAZYSPARKS_SIZE	(METERTOUNIT(0.025f))
#define CRAZYSPARKS_DELTA	(0.01f)						// CMD: for a velocity of 1 m/s.

#define CRAZYSPARKS_TRAILVEL	(1<<0)

typedef struct TAG_ELECTRICITYPATHINFO
{
	int				numObjects;
	int				currentObject;
	uint32			*objectNames;
	int				pad;
} ELECTRICITYPATHINFO BALIGN16;

typedef struct TAG_ELECTRICITYINFO
{
	TBVector			currentTarget;

	ELECTRICITYPATHINFO	*pathNodes;
	float				nodeInterceptAccuracy;
	char				hasStartPosition;					//has a start position been chosen for the spark yet
	char				state;								//whether sparks are on or not
	char				pad[2];
	float				speed;
} ELECTRICITYINFO BALIGN16;

typedef struct TAG_CRAZYSPARKSINFO
{
	TBVector		leadPosition;
	TBVector		trailPosition;
	TBVector		velocity;
	TBVector		trailVelocity;

	float			alpha;
	float			trailClock;
	float			life;
	float			size;

	TBTexture		*texture;
	float			angle;
	uint32			flags;
	int32			iPad;
}CRAZYSPARKSINFO, _CRAZYSPARKSINFO BALIGN16;

extern TBExtraSystem	*crazySparksExtrasSystem;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateElectricityObject(float nodeInterceptAccuracy, char *objectName, char *name, TBVector scale, char state, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateElectricityInfoStructure(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateElectricityObject(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : CalculateStartPosition
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void CalculateSparkStartPosition(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : CalculateNewSparkTarget
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void CalculateNewSparkTarget(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : MoveSparkTowardsTarget
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void MoveSparkTowardsTarget(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : UpdateSparkOrientation
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void UpdateSparkOrientation(ACTORINSTANCE *actor, TBVector velocity);

/* --------------------------------------------------------------------------------
   Function : CreateElectricityPathInfo
   Purpose : 
   Parameters : ELECTRICITYPATHINFO *
   Returns : ELECTRICITYPATHINFO *
   Info : 
*/

ELECTRICITYPATHINFO *CreateElectricityPathInfo(int numObjects);

/* --------------------------------------------------------------------------------
   Function : AddElectricityPathObject
   Purpose : 
   Parameters : ELECTRICITYPATHINFO *, char *
   Returns : 
   Info : 
*/

int AddElectricityPathObject(ELECTRICITYPATHINFO *pathInfo, uint32 objectName);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeAllElectricityPathNodes(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : WereGonnaHaveSomeCrazySparks
   Purpose : Creates sparks in a Crazy Taxi Stylee, which are used for indestructibles
   Parameters : 
   Returns : 
   Info : 
*/

void WereGonnaHaveSomeCrazySparks(TBVector position1, TBVector position2, float distFromPosition1, TBTexture *texture);

/* --------------------------------------------------------------------------------
   Function : InitCraziSparksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCrazySparksExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CrazySparksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* -------------------------------------------------------------------------------
   Function : CrazySparksPhoneBoxExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksPhoneBoxExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif