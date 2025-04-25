#ifndef __H_PRECIPITATION
#define __H_PRECIPITATION

#define	MAX_PRECIPITATION	(25)
#define MAX_DRAWPRECIP		(250)
#define	SNOW_MAXALPHA		(70.0f)
#define	PRECIPITATION_BOX	(METERTOUNIT(10.0f))
#define	SNOW_VELOCITY	(METERTOUNIT(-1.0f)) 	//free fall speed of snow as estimated by npettitt, any compaints, therefore, should be directed his way
												//(any complaints should be directed his way anyway, whatever they are)
#define	SNOW_ROTATION	(90)					//angular velocity around helical path(indegrees)
#define	RAIN_VELOCITY	(METERTOUNIT(-10.0f))

typedef struct TAG_PRECIPITATIONLISTENTRY
{
	TBVector	bounds;
	TBVector	windVelocity;

	int			density;
	float		drawDistance;
	float		xDim;
	float		yDim;

	uint32		type;
	float		rate;
	TAG_PRECIPITATIONLISTENTRY	*next;
	TAG_PRECIPITATIONLISTENTRY	*prev;

	float		currentTime;
	int			iPad[3];
} PRECIPITATIONLISTENTRY, _PRECIPITATIONLISTENTRY BALIGN16;

typedef struct TAG_PRECIPITATIONLIST
{
	PRECIPITATIONLISTENTRY		head;

	uint32						numEntries;
	uint32						pad[3];
} PRECIPITATIONLIST, _PRECIPITATIONLIST BALIGN16;

typedef struct TAG_PRECIPITATION
{
	TBVector		position;
	TBVector		velocity;
	TBVector		bounds;						//bounds of precipitation field
	TBVector		rotationSpeed;

	TBVector		windVelocity;

	float			radius;						//radius of helix, in which precipitation falls
	float			rotSpeed;					//speed of rotation arround helix
	uint32			density;					//number created per box, per frame
	float			drawDistance;				//distance at which precipitation is faded

	float			xDim;						//x dimension of sprites
	float			yDim;
	TBTexture		*sprite;					// CMD: TBTexture*	
	float			iPad;			

	int				xNumBoxes;
	int				zNumBoxes;
	float			alpha;
	float			rotation;					// CMD: rotation of sprite in 2d space

	float			u, v;
	float			life;
	uint32			flags;
} PRECIPITATION, _PRECIPITATION BALIGN16;

extern	PRECIPITATIONLIST		precipitationList;

/* --------------------------------------------------------------------------------
   Function : InitPrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitPrecipitationList(void);

/* --------------------------------------------------------------------------------
   Function : AddEntryToPrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToPrecipitationList(TBVector bounds, TBVector windVelocity, int density, float drawDistance, float xDim, float yDim, uint32 type, float rate);

/* --------------------------------------------------------------------------------
   Function : UpdatePrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePrecipitationList(void);

/* --------------------------------------------------------------------------------
   Function : CreateSnow
   Purpose : Creates tiled boxes of randomly generated snow sprites
   Parameters : bounds = {xMin, xMax, zMin, zMax};
   Returns : 
   Info : 
*/

int CreatePrecipitation(TBVector bounds, TBVector windVelocity, int density, float drawDistance, float xDim, float yDim, uint32 type, float rate, PRECIPITATIONLISTENTRY *listEntryPtr);

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraCreateCallback
   Purpose : This function is called when precipitation is created
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraUpdateCallback
   Purpose : This function is called each frame to update the precipitation
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : UpdateSnow
   Purpose : updates the position of snow
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSnow(PRECIPITATION *snow);

/* --------------------------------------------------------------------------------
   Function : UpdateRain
   Purpose : updates the position of rain
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRain(PRECIPITATION *rain);

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FreeAllPrecipitationFromList
   Purpose : Removes all actors from the list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPrecipitationFromList(void);

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraDeleteCallback
   Purpose : This function is called when precipitation extras are deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PrecipitationExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void PrecipitationExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif