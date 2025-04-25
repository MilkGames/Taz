#ifndef __H_WATERSPLASH
#define __H_WATERSPLASH

#define	MAX_SPLASHES	(100)
#define	HORIZ_SPLASH_SPEED	(175.0f)
#define	VERT_SPLASH_SPEED	(600.0f)
#define NUM_WATER_SPLASH_PARTS	(15)
#define	SPLASH_LEAD_TIME	(0.1f)
#define	SPLASH_LIFETIME		(2.0f)

// CMD: flags
#define	WATERSPLASH_CREATERIPPLE		(1<<0)

typedef struct TAG_SPLASHINFO
{
	TBVector		position;
	TBVector		velocity;

	struct
	{
		int			r, g, b;
		float		a;
	}colour;

	float			size;
	float			height;
	uint32			flags;
	int32			iPad;

}SPLASHINFO, _SPLASHINFO BALIGN16;

typedef struct TAG_SQUISHEXTRAINFO
{
	TBVector		leadingPos;
	TBVector		trailingPos;
	TBVector		velocity;

	float			leadingDim;
	float			trailingDim;
	float			life;
	float			alpha;
}SQUISHEXTRAINFO, _SQUISHEXTRAINFO BALIGN16;

extern TBExtraSystem		*squishExtraSystem;

/* --------------------------------------------------------------------------------
   Function : InitSplashExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void CreateWaterSplashes(TBVector position, float scale=1.0f);

/* --------------------------------------------------------------------------------
   Function : InitSplashExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitSplashExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : SplashExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void SplashExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : SplashExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SplashExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : SplashExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void SplashExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : SplashExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SplashExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : SplashExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void SplashExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : IntVectorCopy
   Purpose : copies a vector made up from integers
   Parameters : 
   Returns : 
   Info : 
*/

void IntVectorCopy(int dest[4], int source[4]);

/* --------------------------------------------------------------------------------
   Function : InitFountainExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitSquishExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : SquishExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : leadingPosition, leadingDim, trailingDim, height, FOUNTAINEXTRA_WAYDOWN);
   Info : 
*/

void SquishExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : SquishExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SquishExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : SquishExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : SquishExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : SquishExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif