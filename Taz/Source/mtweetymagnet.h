#ifndef __H_MTWEETYMAGNET
#define __H_MTWEETYMAGNET

#define MAX_MANGABLOBS		(500)
#define NUM_MAGNET_POINTS	(10)
#define NUM_MAGNET_STREAKS	(8)
#define MAGNET_MAX_FLICKER	(METERTOUNIT(0.1f))

//magnet flags
#define MAGNETFLAG_SOURCE	(1<<0)
#define MAGNETFLAG_TWEETY	(1<<1)
#define MAGNETFLAG_LEFT		(1<<2)
#define MAGNETFLAG_RIGHT	(1<<3)
#define MAGNETFLAG_GOOD		(1<<4)
#define MAGNETFLAG_BAD		(1<<5)

typedef struct TAG_MAGNETSTREAK
{
	TBVector		start, jitter;
	TBVector		control1, initC1;
	TBVector		control2, initC2;

	TBVector		points[NUM_MAGNET_POINTS];
	float			size[NUM_MAGNET_POINTS];
	int				colour[NUM_MAGNET_POINTS][4];
	TBPrimVertex2D	buffer[(NUM_MAGNET_POINTS*4)+4];

	TBActorNodeInstance		*node;
	float					clock;
	float					jitterClock;
	TBTexture				*texture;
} MAGNETSTREAK, _MAGNETSTREAK BALIGN16;

typedef struct TAG_MANGAMAGNETINFO
{
	TBVector		position;
	TBVector		velocity;

	int				colour[4];

	float			size;
	float			heightFlicker;
	float			distanceRemaining;
	float			clock;

	float			upperAlpha;
	uint32			iPad[2];
	uint32			flags;
} MANGAMAGNETINFO, _MANGAMAGNETINFO BALIGN16;

extern TBExtraSystem	*mangaMagnetExtraSystem;
extern TBPrimVertex2D	*mangaMagnetDrawBuffer;
extern TBPrimVertex2D	*badMangaMagnetDrawBuffer;


/* --------------------------------------------------------------------------------
   Function : InitMangaMagnetExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/ 
void InitMangaMagnetExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int MangaMagnetExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void MangaMagnetExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : InitialiseMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int InitialiseMagnetStreaks(struct TAG_MECHATWEETYINFO *info);

/* --------------------------------------------------------------------------------
   Function : UpdateMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMagnetStreaks(struct TAG_MECHATWEETYINFO *info);

/* --------------------------------------------------------------------------------
   Function : DrawMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMagnetStreaks(struct TAG_MECHATWEETYINFO *info);

/* --------------------------------------------------------------------------------
   Function : FreeMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeMagnetStreaks(struct TAG_MECHATWEETYINFO *info);

#endif