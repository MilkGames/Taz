#ifndef __H_GLOWAROUNDPLINTH
#define __H_GLOWAROUNDPLINTH

#include "actors.h"

#define MAX_PLINTHBUBBLES		(350)
#define PLINTHGLOW_FADETIME		(1.0f)
#define PLINTHGLOW_NUMWIGGLES	(3)
#define POLICEGLOW_NUMWIGGLES	(6)

// CMD: flags
#define PLINTHFLAGS_VELOCITY	(1<<0)

typedef struct TAG_SIMPLEBOUNDINGBOX
{
	float		xmin, xmax, ymin, ymax, zmin, zmax;
	int32		ipad[2];
}SIMPLEBOUNDINGBOX, _SIMPLEBOUNDINGBOX BALIGN16;

typedef struct TAG_PLINTHWIGGLES
{
	float		radius;
	float		clock;
	float		theta;
	float		thetaVel;

	float		phi;
	float		phiVel;
	float		radiusDelta;
	int32		iPad;
} PLINTHWIGGLES, _PLINTHWIGGLES BALIGN16;

typedef struct TAG_PLINTHGLOWINFO
{
	TBVector			position;

	PLINTHWIGGLES		wiggles[PLINTHGLOW_NUMWIGGLES];
	SIMPLEBOUNDINGBOX	bb;

	float				clock;
	int					alpha;
	void				*plinth;
	int32				iPad;
}PLINTHGLOWINFO, _PLINTHGLOWINFO BALIGN16;

typedef struct TAG_POLICEGLOWINFO
{
	TBVector			position;

	PLINTHWIGGLES		wiggles[POLICEGLOW_NUMWIGGLES];
	SIMPLEBOUNDINGBOX	bb;

	float				clock;
	int					alpha;
	void				*plinth;
	int32				iPad;
}POLICEGLOWINFO, _POLICEGLOWINFO BALIGN16;

typedef struct TAG_PLINTHEXTRAINFO
{
	TBVector	position;
	TBVector	velocity;

	int			alpha;
	float		alphaFadeRate;
	float		size;
	float		clock;

	uint32		flags;
	uint32		iPad[3];
} PLINTHEXTRAINFO, _PLINTHEXTRAINFO BALIGN16;

extern PLINTHGLOWINFO	*zooHubPlinthGlow;
extern POLICEGLOWINFO	hubPoliceBoxGlow;
extern TBExtraSystem	*plinthGlowExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseGlowAroundPlinth(void);

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundPlinthInZooHub(void);


/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundSecretItem(void	*context);

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundInstance(ACTORINSTANCE	*actorInstance, float yOffset=0.0f);

/* --------------------------------------------------------------------------------
   Function : UpdateGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateGlowAroundPlinth(PLINTHGLOWINFO *plinthArray);

/* --------------------------------------------------------------------------------
   Function : UpdateGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePlinthGlowAroundActorInstance(PLINTHGLOWINFO *info);

/* --------------------------------------------------------------------------------
   Function : DrawGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawGlowAroundPlinth(PLINTHGLOWINFO *plinthArray);

/* --------------------------------------------------------------------------------
   Function : DrawGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeGlowAroundPlinth(PLINTHGLOWINFO *data);

/* --------------------------------------------------------------------------------
   Function : InitPlinthGlowSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitPlinthGlowExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PlinthGlowExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void PlinthGlowExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

void StartPoliceBoxGlowEffect(ACTORINSTANCE *actorInstance);
void EndPoliceBoxGlowEffect();

#endif