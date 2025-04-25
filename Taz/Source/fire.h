#ifndef __H_FIRE
#define __H_FIRE

#define	FIRE_PARTICLE_PERIOD	(0.25f)
#define FIRE_MAXVERTICALSPEED	(METERTOUNIT(1.0f))

#define	MAX_FIRE				(250)

enum
{
	FIRETYPE_YELLOW,
	FIRETYPE_RED,
};

typedef struct TAG_FIREINFO
{
	float		maxRadius;
	TBTexture	*texture;
	float		clock;
	
	struct 
	{
		float	xmin, xmax, ymin, ymax, zmin, zmax;
	} bb;
} FIREINFO, _FIREINFO;

typedef struct TAG_FIREEXTRAS
{
	TBVector		initPos;

	TBVector		position;
	
	TBVector		velocity;

	TBPrimVertex	vertexList[6];

	TBMatrix		localToWorld;

	float			life;
	TBTexture		*texture;
	float			scale;
	float			rotation;

	int				colour[4];

	int				type;
	int				iPad[3];
} FIREEXTRAS, _FIREEXTRAS BALIGN16;

typedef struct TAG_FIRERENDERINFO
{
	TBMatrix		localToWorld;

	TBVector		position;

	int				colour[4];

	float			scale;
	float			rotation;
	int				pad[2];
} FIRERENDERINFO, _FIRERENDERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateFireInstance
   Purpose : creates structure to hold fire instance information
   Parameters :
   Returns : true or false: succesfull or not
   Info : 
*/

int CreateFireInstance(struct TAG_ACTORINSTANCE *fireInstance, float size);

/* --------------------------------------------------------------------------------
   Function : CreateFireInstance
   Purpose : creates structure to hold fire instance information
   Parameters :
   Returns : true or false: succesfull or not
   Info : 
*/

FIREINFO *CreateFireInfo(void);

/* --------------------------------------------------------------------------------
   Function : UpdateFireInstances
   Purpose : updates all the fire instances on a map
   Parameters :
   Returns : 
   Info : 
*/

void UpdateFireInstances(struct TAG_ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : CreateFireParticleEffect
   Purpose : creates the actual fire effect
   Parameters :
   Returns : 
   Info : 
*/

void CreateFireParticleEffect(struct TAG_ACTORINSTANCE *fireInstance);

/* --------------------------------------------------------------------------------
   Function : CreateFireExtrasVertexList
   Purpose : creates the actual fire effect
   Parameters :
   Returns : 
   Info : 
*/

void CreateFireExtrasVertexList(FIREEXTRAS *fireInfo);

/* --------------------------------------------------------------------------------
   Function : InitFireExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitFireExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : FireExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : FireExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FireExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : FireExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : FireExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FireExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

void InitFireInstanceList(void);

/* --------------------------------------------------------------------------------
   Function : TBPrimVertexCopy
   Purpose : This function copies a TBPrimVertex structure
   Parameters : 
   Returns : 
   Info : 
*/

void TBPrimVertexCopy(TBPrimVertex *dest, TBPrimVertex *source);
#endif