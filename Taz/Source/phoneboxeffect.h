#ifndef	__H_PHONEBOXEFFECT
#define	__H_PHONEBOXEFFECT

#define	MAX_PHONEBOXEFFECTS		(6)
#define	PHONEFX_NUMNODES		(16)
#define	PHONEFX_ORANGETIME		(10.0f)
#define	PHONEFX_WHITETIME		(10.0f)
#define	PHONEFX_ANGVELOCITY		(RAD(180.0f))
#define PHONEFX_MAXVERTSPEED	(METERTOUNIT(2.0f))
#define	PHONEFX_RADIALVELOCITY	(METERTOUNIT(0.0f))
#define PHONEFX_FADERATE		(10.0f)
#define	PHONEFX_ANGULAROFFSET	(RAD(15.0f))

typedef struct TAG_PHONEBOXEFFECTINFO
{
	TBActorNodeInstance		*node;
	float					clock;
	TBTexture				*orange;
	TBTexture				*white;
} PHONEBOXEFFECTINFO, _PHONEBOXEFFECTINFO BALIGN16;

typedef struct TAG_PHONEBOXEXTRAS
{
	TBVector		initialPosition;
	TBVector		nodes[PHONEFX_NUMNODES];

	float			currentRot[PHONEFX_NUMNODES];
	float			height[PHONEFX_NUMNODES];
	float			size[PHONEFX_NUMNODES];
	float			radius[PHONEFX_NUMNODES];
	int				colour[PHONEFX_NUMNODES][4];

	TBTexture		*texture;
	float			angularVel;
	float			verticalSpeed;
	float			radialVelocity;

	float			life;
	int				iPad[3];
} PHONEBOXEXTRAS, _PHONEBOXEXTRAS BALIGN16;

/* --------------------------------------------------------------------------------
   Function : StartPhoneBoxEffect
   Purpose : call this function to start the effect
   Parameters : 
   Returns : 
   Info : 
*/

int StartPhoneBoxEffect(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBoxEffect
   Purpose : call this function to update the effect
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePhoneBoxEffect(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : EndPhoneBoxEffect
   Purpose : call this function to end the effect
   Parameters : 
   Returns : 
   Info : 
*/

void EndPhoneBoxEffect(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitFireExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitPhoneBoxExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PhoneBoxExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : FireExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxPhoneBoxExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif