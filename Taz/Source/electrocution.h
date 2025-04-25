#ifndef __H_ELECTROCUTION
#define __H_ELECTROCUTION

#define ELECTRIC_BOUNCE_SPEED	(METERTOUNIT(3.0f))
#define ELECTRIC_RESET_TIME		(5.0f);

//Taz sparks extra system defines
#define MAX_TAZ_SPARKS		(20)

enum
{
	HIGHVOLTAGE_OFF,
	HIGHVOLTAGE_WALL,
	HIGHVOLTAGE_GROUND,
};

typedef struct TAG_HIGHVOLTAGEOBJECTINFO
{
	int				currentState;
	int				possibleState;
	struct TAG_TRIGGERINFO		*onContact;
	float			resetTime;
} HIGHVOLTAGEOBJECTINFO, _HIGHVOLTAGEOBJECTINFO BALIGN16;

//structure for info pertaining to sparks emanating from taz
typedef struct TAG_TAZSPARKSINFO
{
	TBVector			velocity;

	TBActorInstance		*actorInstance;
	int				alpha;
	float			life;
	int				iPad;
} TAZSPARKSINFO, _TAZSPARKSINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateHighVoltageObject
   Purpose : creates an object which can, if turned on, electrocute a character
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int CreateHighVoltageObject(ACTORINSTANCE *actor, int type, char state);

/* --------------------------------------------------------------------------------
   Function : CreateHighVoltageInfo
   Purpose : creates info structure
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int CreateHighVoltageInfo(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : UpdateHighVoltageObject
   Purpose : updates high voltage objects
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void UpdateHighVoltageObject(ACTORINSTANCE *actor);

/* --------------------------------------------------------------------------------
   Function : RespondToHighVoltageCollision
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int RespondToHighVoltageCollision(ACTORINSTANCE *collider, ACTORINSTANCE *object);

/* --------------------------------------------------------------------------------
   Function : UpdateElectrocutionEffect
   Purpose : 
   Parameters :
   Returns :
   Info :
*/

void UpdateElectrocutionEffect(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitTazSparksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitTazSparksExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : TazSparksExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void TazSparksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : TazSparksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int TazSparksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : TazSparksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void TazSparksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : TazSparksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void TazSparksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : TazSparksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void TazSparksExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif