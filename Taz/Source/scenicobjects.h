#ifndef	__H_SCENICOBJECTS
#define	__H_SCENICOBJECTS


#include "actors.h"

enum EScenic
{
	SCENICTYPE_NONE,
	SCENICTYPE_FIREWORKS,
};

typedef struct TAG_SCENICINFO
{
	uint32		defaultAnimationCRC;
	uint32		smokeNodeCRC;
	float		maxScale;
	EScenic		type;
} SCENICINFO, _SCENICINFO BALIGN16;

#define NUM_SCENIC_FIREWORKS	(100)

typedef struct TAG_SCENICFIREWORKS
{
	TBVector		position;

	float			alpha;
	float			size;
	int32			iPad[2];
} SCENICFIREWORKS, _SCENICFIREWORKS BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateScenic
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateScenic(ACTORINSTANCE *actorInstance, char *animation, int collision, uint32 smokeNodeCRC, float maxScale, EScenic type=SCENICTYPE_NONE, int liveupdate=0, bool spring = false);

/* --------------------------------------------------------------------------------
   Function : CreateScenicInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

SCENICINFO *CreateScenicInfo(void);

/* --------------------------------------------------------------------------------
   Function : CreateScenicInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateScenicObject(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitScenicFireworksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitScenicFireworksExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void ScenicFireworksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ScenicFireworksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif