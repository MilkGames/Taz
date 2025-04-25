#ifndef __H_HAAR
#define __H_HAAR

#define	MAX_HAAR		(10)
#define HAAR_SPEED		(METERTOUNIT(0.1f))
#define HAAR_TILE		(1.0f)
#define	HAAR_SCALE		(10.0f)
#define HAAR_ALPHA		(60.0f)


typedef struct TAG_HAARINFO
{
	TBPrimVertex	verts[6];
	TBVector	direction;					

	float		density;
	float		clock;
	int32		iPad[2];
} HAARINFO, _HAARINFO BALIGN16;

typedef struct TAG_HAAREXTRAINFO
{
	TBPrimVertex	verts[6];
	TBVector		position;
	TBVector		velocity;
	
	float			life;
	float			scale;
	uint32			alphaState;
	float			density;

	float			u[6];
	float			v[6];

	float			r, g, b, a;
} HAAREXTRAINFO, _HAAREXTRAINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateHarrObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

int CreateHaarObject(struct TAG_ACTORINSTANCE *actorInstance, float density, float r, float g, float b);

/* --------------------------------------------------------------------------------
   Function : UpdateHaarObject
   Purpose : updates the object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHaarObject(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseRandomPositionWithinBoundingBox
   Purpose : chooses a random position within the bounding box of an actor
   Parameters : 
   Returns : 
   Info : 
*/

void ChooseRandomPositionWithinBoundingBox(TBVector dest, TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitHaarExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitHaarExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : HaarExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : HaarExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int HaarExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : HaarExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void HaarExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif