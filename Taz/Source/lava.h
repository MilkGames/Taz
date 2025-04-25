#ifndef __H_LAVA
#define __H_LAVA

#define MAX_LAVASPITTLE		(880)
#define LAVASPIT_HORIZVEL	(METERTOUNIT(1.5f))
#define LAVASPIT_VERTVEL	(METERTOUNIT(4.0f))
#define LAVASPIT_INITR		(115.0f)
#define LAVASPIT_INITG		(15.0f)
#define LAVASPIT_INITB		(15.0f)
#define LAVASPIT_TARGETR	(127.0f)
#define LAVASPIT_TARGETG	(60.0f)
#define LAVASPIT_TARGETB	(30.0f)

enum ELavaState
{
	LAVASPIT_IDLE,
	LAVASPIT_TRAIL,
	LAVASPIT_FOUNTAIN,
};

typedef struct TAG_LAVAOBJECTINFO
{
	bool	smoke;			// TP: Do you want to generate smoke
} LAVAOBJECTINFO, _LAVAOBJECTINFO;

typedef struct TAG_LAVASPITINFO
{
	TBVector	position;
	TBVector	offset;
	TBVector	velocity;

	float		clock;
	ELavaState	state;
}LAVASPITINFO, _LAVASPITINFO;

typedef struct TAG_LAVASPITEXTRA
{
	TBVector	position;
	TBVector	velocity;

	float		alpha;
	float		alphaFadeRate;
	float		xSize;
	float		ySize;

	float		r, g, b;
	ELavaState	type;
}LAVASPITEXTRA, _LAVASPITEXTRA BALIGN16;
 
extern TBExtraSystem	*lavaSpitExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/
int CreateLavaObject(struct TAG_ACTORINSTANCE *actorInstance,bool smoke);

/* --------------------------------------------------------------------------------
   Function : RespondToLavaCollision
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/
int RespondToLavaCollision(struct TAG_ACTORINSTANCE *collider, ACTORINSTANCE *object);

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void CreateLavaSpitSource(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void UpdateLavaSpitSource(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitLavaSpitExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitLavaSpitExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int LavaSpitExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void LavaSpitExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif