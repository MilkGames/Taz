#ifndef	__H_FOUNTAINEXTRASYSTEM
#define	__H_FOUNTAINEXTRASYSTEM

#define MAX_FOUNTAINSPLASHES		(300)
#define	FOUNTAINEXTRA_WAYUPSPEED	(METERTOUNIT(24.0f))
#define FOUNTAINEXTRA_UPSPEED		(METERTOUNIT(8.0f))
#define FOUNTAINEXTRA_HORIZSPEED	(1000)

enum
{
	FOUNTAINEXTRA_WAYUP,
	FOUNTAINEXTRA_WAYDOWN,
};

typedef struct TAG_FOUNTAINEXTRAINFO
{
	TBVector		leadingPos;

	TBVector		trailingPos;

	TBVector		velocity;

	float			leadingDim;
	float			trailingDim;
	float			height;
	int				state;

	TBTexture		*texture;
	float			life;
	struct TAG_ACTORINSTANCE	*instanceOnFountain;
	int				iPad;
} FOUNTAINEXTRAINFO, _FOUNTAINEXTRAINFO BALIGN16;

extern TBExtraSystem	*fountainExtraSystem;

/* --------------------------------------------------------------------------------
   Function : InitFountainExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitFountainExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : FountainExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void FountainExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : FountainExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FountainExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : FountainExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : FountainExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FountainExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif