#ifndef __H_ZZZ
#define __H_ZZZ

#define	MAX_ZZZ			(7)
#define ZZZ_PERIOD		(0.5f)
#define	ZZZ_RADIUSRATE	(METERTOUNIT(0.1f))
#define ZZZ_ANGULARVEL	(RAD(60.0f))
#define ZZZ_SCALERATE	(METERTOUNIT(0.025f))

#define QUESTIONMARK_FADERATE	(255.0f)

enum
{
	QUESTIONMARKSTATE_FADEUP,
	QUESTIONMARKSTATE_STEADY,
	QUESTIONMARKSTATE_FADEDOWN,
};

typedef struct TAG_ZZZINFO
{
	TBTexture	*texture;
	TBTexture	*altTexture;
	TBActorNodeInstance	*originNode;
	float		clock;

	float		size;
	int			flags;		// TP: 0 normal, 1 cheese mod
	int			iPad[2];
} ZZZINFO, _ZZZINFO BALIGN16;

typedef struct TAG_ZZZEXTRAINFO
{
	TBVector	position;

	TBTexture	*texture;
	float		scale;
	float		angle;
	float		life;

	float		radius;
	float		height;
	float		alpha;
	int			iPad;
} ZZZEXTRAINFO, _ZZZEXTRAINFO BALIGN16;

typedef struct TAG_QUESTIONMARKINFO
{
	TBVector	position;
	TBVector	offset;

	TBTexture	*texture;
	float		timer;
	int			alpha;
	float		size;

	TBActorNodeInstance	*bone;
	int			state;
	float		pulseInc;
	float		falpha;
} QUESTIONMARKINFO, _QUESTIONMARKINFO BALIGN16;

extern TBExtraSystem	*zzzExtraSystem;

/* --------------------------------------------------------------------------------
   Function : ActivateSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ActivateSleepyZedsForCharacter(ACTORINSTANCE *actorInstance, char *bone, TBTexture *texture, TBTexture *altTexture, float size);

/* --------------------------------------------------------------------------------
   Function : TurnOffSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void TurnOffSleepyZedsForCharacter(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSleepyZedsForCharacter(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : InitZzzExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitZzzExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : ZzzExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : ZzzExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ZzzExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : ZzzExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : ZzzExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : ZzzExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : CreateQuestionMarkAboveActor
   Purpose : Creates question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

int CreateQuestionMarkAboveActor(struct TAG_ACTORINSTANCE *actorInstance, char *bone, char *texture, float size, TBVector offset);

/* --------------------------------------------------------------------------------
   Function : DestroyQuestionMarkAboveActor
   Purpose : Destroys the question mark above an actors head
   Parameters : 
   Returns : true if there was a question mark
   Info : 
*/

int DestroyQuestionMarkAboveActor(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateQuestionMarkAboveActor
   Purpose : Updates the question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateQuestionMarkAboveActor(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawQuestionMarkAboveActor
   Purpose : Draws the question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

void DrawQuestionMarkAboveActor(struct TAG_ACTORINSTANCELIST *actorInstanceList);

#endif