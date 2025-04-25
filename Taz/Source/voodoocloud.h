#ifndef __H_VOODOOCLOUD
#define __H_VOODOOCLOUD

#include "parsemap.h"
#include "lightning.h"

#define	VOODOOCLOUD_IDLETIME	(5.0f)
#define MAX_VOODOORAIN			(20)

enum
{
	VOODOOCLOUDSTATE_APPEAR,
	VOODOOCLOUDSTATE_DISAPPEAR,
	VOODOOCLOUDSTATE_PASSIVE,
	VOODOOCLOUDSTATE_LIGHTNING,
	VOODOOCLOUDSTATE_CLANGER,
	VOODOOCLOUDSTATE_HAT,
};

typedef struct TAG_VOODOOCLOUDRAININFO
{
	TBVector		position;

	float			velocity;
	float			xOffset;
	float			zOffset;
	float			fPad;

	int				red;
	int				green;
	int				blue;
	int				alpha;

	float			xDim;
	float			yDim;
	TBTexture		*texture;
	float			life;
} VOODOOCLOUDRAININFO, _VOODOOCLOUDRAININFO BALIGN16;

typedef struct TAG_VOODOOCLOUDINFO
{
	struct TAG_LIGHTNINGINFO	lightningInfo;	

	TBActorInstance		actorInstance;

	int			state;
	float		clock;
	float		offset;
	float		rainClock;
} VOODOOCLOUDINFO, _VOODOOCLOUDINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateVoodooCloud
   Purpose : creates cloud for voodoocloud attack
   Parameters : 
   Returns : 
   Info : 
*/

int CreateVoodooCloud(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateVoodooCloudInfo
   Purpose : creates cloud for voodoocloud attack
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOCLOUDINFO *CreateVoodooCloudInfo(void);

/* --------------------------------------------------------------------------------
   Function : PrepVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void PrepVoodooCloud(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateVoodooCloud(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawVoodooCloud(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitVoodooCloudExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitVoodooCloudExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void VoodooCloudExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int VoodooCloudExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : SplashExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/*  --------------------------------------------------------------------------------
	Function	: DeleteVoodooCloud
	Purpose		: Called to remove the cloud from above taz's head
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DeleteVoodooCloud(ACTORINSTANCE *actorInstance);

#endif