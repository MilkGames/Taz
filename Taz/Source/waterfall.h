#ifndef	__H_WATERFALL
#define	__H_WATERFALL

#define MAX_WATERFALLPARTS		(100)
#define WATERFALL_PERIOD		(0.2f)
#define WATERFALL_LIFE			(3.0f)
#define WATERFALL_DELTA			(WATERFALL_LIFE*0.1f)

#define WATERFALL_SPRAYVEL		(METERTOUNIT(0.5f))
#define WATERFALL_SPRAYANGVEL	(3)
#define WATERFALL_SPRAYSCALEVEL	(METERTOUNIT(1.0f))

enum
{
	WATERFALL_SPLASH,
	WATERFALL_SPRAY,
};

typedef struct TAG_WATERFALLINFO
{
	TBVector	velocity;

	float		clock;
	TBTexture	*texture;
	TBTexture	*highlight;
	uint32		ipad;

	int			soundHandle;

//bottom spray
	TBVector	sprayPosition;
	TBVector	sprayEndVector;

	TBTexture	*spray;
	int32		vectorLen;
	uint32		sprayStart;
	uint32		sprayEnd;
} WATERFALLINFO, _WATERFALLINFO BALIGN16;

typedef struct TAG_WATERFALLEXTRAINFO
{
	TBPrimVertex	verts[4];
	TBVector		velocity;
	TBVector		trailVelocity;
	TBVector		position;

	float			life;
	float			trailClock;
	TBTexture		*texture;

	int32			updateTrail;

	float			horizSpeedFactor;
	uint32			alphaState;
	uint32			type;
	float			alpha;

//bottom spray
	float		angVel;
	float		angle;
	float		xDim;
	int32		iPad;
} WATERFALLEXTRAINFO, _WATERFALLEXTRAINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateWaterfallInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateWaterfallInstance(ACTORINSTANCE *actorInstance, uint32 textureName, uint32 sprayStart, uint32 sprayEnd);

/* --------------------------------------------------------------------------------
   Function : UpdateWaterfallInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterfallInstance(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateWaterfallVerts
   Purpose : creates verts for waterfall fragment
   Parameters : 
   Returns : 
   Info : 
*/

void CreateWaterfallVerts(WATERFALLINFO *info, TBVector position, TBVector scale);

/* --------------------------------------------------------------------------------
   Function : InitWaterfallExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitWaterfallExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int WaterfallExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void WaterfallExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif