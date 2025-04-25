#ifndef __H_EXPLODE
#define __H_EXPLODE

#define MAX_EXPLOSION	(10)

enum
{
	EXPLOSIONTYPE_CARTOON,
};

typedef struct TAG_EXPLOSION
{
	TBVector		scale;

	uint32			objectNameCRC;
	float			rateOfScale;
	int				rateOfFade;			// PP: determines how quickly the explosion fades to transparency
	int				alpha;
	TBActorInstance	*actorInstance;
} EXPLOSION, _EXPLOSION BALIGN16;

// PP: default destination scale for explosions
#define EXPLODE_DEFAULT_FADE_RATE		100

/* --------------------------------------------------------------------------------
   Function : StartExplosion
   Purpose : initiates an explosion, and add it to extra system
   Parameters : position, initial scale, rate of scale, sound sample name, type, (PP)[opt/EXPLODE_DEFAULT_FADE_RATE] rate of fade
   Returns : 
   Info : scale refers to the approximate initial size of the effect
*/

void StartExplosion(TBVector position, float scale, float rateOfScale, char *soundSample, uint32 type, const int rateOfFade=EXPLODE_DEFAULT_FADE_RATE);

/* --------------------------------------------------------------------------------
   Function : StartCartoonExplosion
   Purpose : initiates a cartoon explosion, and add it to extra system
   Parameters : position, initial scale, rate of scale, (PP)[opt/EXPLODE_DEFAULT_FADE_RATE] rate of fade
   Returns : 
   Info : 
*/

void StartCartoonExplosion(TBVector position, float scale, float rateOfScale, const int rateOfFade=EXPLODE_DEFAULT_FADE_RATE);

/////////////////// explosion extra system functions ///////////////////

/* --------------------------------------------------------------------------------
   Function : InitExplosionExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitExplosionExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ExplosionExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraUpdateCallback
   Purpose : This function is called each frame to update the debris
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraRenderCallback
   Purpose : This function is called each frame to render the debris
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PP: NOTE: I've removed the poster explosion stuff - I'm using Fireball.cpp/.h now.
// PP: Just in case you maybe thought this was an out-of-date version of the file.
// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif// PP: ndef __H_EXPLODE