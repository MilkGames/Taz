#ifndef __H_FIREWORKS
#define __H_FIREWORKS

#define MAX_FIREWORKS	(200)
#define PARTICLES_PER_FIREWORK	(50)

#define FIREWORKS_SPARKLEVELOCITY	(1500)		// CMD: in units per second
#define	FIREWORKS_SPARKLELIFE		(3.0f)
#define FIREWORKS_SPARKLEDECAY		(43.0f)
#define FIREWORKS_SPARKLEDECEL		(FIREWORKS_SPARKLEVELOCITY*-0.5f)
#define FIREWORKS_SPARKLESIZE		(METERTOUNIT(0.4f))

#define FIREWORKS_STREAKVELOCITY	(METERTOUNIT(10.0f))
#define FIREWORKS_STREAKLIFE		(2.0f)
#define FIREWORKS_STREAKDECAY		(50.0f)
#define FIREWORKS_STREAKDECEL		(METERTOUNIT(2.0f))
#define FIREWORKS_STREAKSIZE		(METERTOUNIT(0.5f))
#define FIREWORKS_STREAKTRAILSIZE	(METERTOUNIT(0.01f))
#define FIREWORKS_STREAKDELAY		(0.25f)

#define FIREWORKS_BANGERVELOCITY	(METERTOUNIT(10.0f))
#define FIREWORKS_BANGERLIFE		(0.75f)
#define FIREWORKS_BANGERSIZE		(METERTOUNIT(1.0f))

#define FIREWORKS_TWISTERLIFE		(1.0f)
#define FIREWORKS_TWISTERVELOCITY	(METERTOUNIT(3.0f))
#define FIREWORKS_TWISTERPERIOD		(0.1f)
#define FIREWORKS_TWISTERTRAILLIFE	(0.5f)
#define FIREWORKS_TWISTERTRAILSIZE	(METERTOUNIT(0.4f))
#define FIREWORK_TWISTERSIZEINCREM	(METERTOUNIT(1.0f))

// CMD: flags
// CMD: note: colours are not completely random, they are chosen from a list, we don't want shit coloured fireworks, do we?
#define FIREWORKS_RANDOMCOLOUR		(1<<0)		// CMD: random colour of burst as a whole
#define FIREWORKS_RANDOMPARTICLE	(1<<1)		// CMD: random particle colour

enum EFirework
{
	FIREWORK_STREAK,
	FIREWORK_SPARKLE,
	FIREWORK_BANGER,
	FIREWORK_TWISTER,

	FIREWORK_NUM_STYLES,

	FIREWORK_SUBBURST,
	FIREWORK_SUBTWISTER,
	FIREWORK_TWISTERTRAIL,
	FIREWORK_RANDOMTYPE,
};

typedef struct TAG_RGBA	
{
	float	r,g,b,a;
	int		sparkleAlpha;
}RGBA, _RGBA BALIGN16;

typedef struct TAG_FIREWORKINFO
{
	TBVector	position[2];				// CMD: [0]position of particle,[1]position of trail, if needed
	TBVector	velocity;

	RGBA		colour[2];				// CMD: colour of particle

	float		life;
	EFirework	type;
	TBTexture	*texture;
	float		streakClock;

	float		size[2];
	float		angle;
	float		twisterClock;

	float		scale;
	int32		iPad[3];
} FIREWORKINFO, _FIREWORKINFO;

extern TBExtraSystem		*fireworksExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateFireworkBurst
   Purpose : Creates a firework burst of a given type at the specified origin
   Parameters : 
   Returns : 
   Info : 
*/

void CreateFireworkBurst(TBVector position, EFirework type, int32 r, int32 g, int32 b, uint32 flags, float scale=1.0f);

/* --------------------------------------------------------------------------------
   Function : InitFireworksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitFireworksExtraSystem(uint32 numExtras=MAX_FIREWORKS);

/* --------------------------------------------------------------------------------
   Function : FireExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FireworksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void FireworksExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif