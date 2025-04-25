#ifndef __H_MECHATWEETY
#define __H_MECHATWEETY

#include "electricityattack.h"
#include "redshield.h"
#include "mtweetymagnet.h"
#include "actors.h"
#include "rumble.h"

#define MTWEETY_SCALE								(2.0f)
#define	MTWEETY_NUMPOINTS							(20)
#define MTWEETY_NUMSTREAKS							(5)
#define MTWEETY_ATTACKDIST							(METERTOUNIT(15.0f))
#define MTWEETY_NUMSCALES							(4)
#define MTWEETY_SCALETAPER							(0.8f)
#define MTWEETY_SCALETAPERRATE						(0.8f)

//generic
#define MTWEETYGENERIC_PANTIME						(5.0f)
#define MTWEETYGENERIC_MAGNETTIME					(3.0f)
#define MTWEETYGENERIC_MINMAGTIME					(0.1f)
#define MTWEETYGENERIC_MAGNETMAXVEL					(METERTOUNIT(4.0f))

#define MTWEETY_NUMHITS								(9)
#define MTWEETY_HELMET_MAXSCALE						(1.0f)

//Taz axis bounds for different stages
#define TAZBOSS_STAGE1_MINX							(-760.0f)
#define TAZBOSS_STAGE1_MAXX							(915.0f)
#define TAZBOSS_STAGE1_MINZ							(-7850.0f)
#define TAZBOSS_STAGE1_MAXZ							(-7585.0f)
#define TAZBOSS_STAGE2_MINX							(-986.00f)
#define TAZBOSS_STAGE2_MAXX							(1030.00f)
#define TAZBOSS_STAGE2_MINZ							(-2470.0f)
#define TAZBOSS_STAGE2_MAXZ							(-1749.0f)
#define TAZBOSS_STAGE3_MINZ							(3073.0f)
#define TAZBOSS_STAGE3_MAXZ							(4492.0f)

//flags
#define MTWEETY_GOTCRATE							(1<<0)
#define MTWEETY_HIGH								(1<<1)
#define MTWEETY_LOW									(1<<2)
#define MTWEETY_THROWCRATE							(1<<3)
#define MTWEETY_VULNERABLE							(1<<4)
#define	MTWEETY_MAGNETCHARGE						(1<<5)
#define MTWEETY_DRAWMAGNETFX						(1<<6)
#define MTWEETY_BADELECTRICITY						(1<<7)
#define MTWEETY_HITBYROCKET							(1<<8)
#define MTWEETY_TWEETYBEATEN						(1<<9)
#define MTWEETY_WAITINGFORBOMBAYDOOR				(1<<10)
#define MTWEETY_NOHURTTAZ							(1<<11)
#define MTWEETY_WAITFORCONTINUE						(1<<12)
#define MTWEETY_DONTUPDATECAMERA					(1<<13)
#define MTWEETY_TWEETYNOMOVE						(1<<14)

enum MTweetyArea
{
	MTWEETYAREA_CRATES,
	MTWEETYAREA_MAGNETS,
	MTWEETYAREA_ROCKETS,
};

enum MTweetyAttackPosition
{
	MTWEETYATTACK_LEFT,
	MTWEETYATTACK_MIDDLE,
	MTWEETYATTACK_RIGHT,
};

enum EMTweetyAIState
{
	MTWEETYAI_IDLE,
	MTWEETYAI_GETCRATE,
	MTWEETYAI_ATTACKCRATE,
	MTWEETYAI_ATTACKELECLEFT,
	MTWEETYAI_ATTACKELECRIGHT,
	MTWEETYAI_ROCKETATTACK,
	MTWEETYAI_DEFEND,
	MTWEETYAI_STUNNED,
	MTWEETYAI_WALKTOTARGET,
	MTWEETYAI_RUNTOTARGET,
	MTWEETYAI_TURNLEFT,
	MTWEETYAI_TURNRIGHT,
	MTWEETYAI_TRANSPORT,
	MTWEETYAI_TRANSPORTOPP,
	MTWEETYAI_MAGNETCHARGE,
	MTWEETYAI_FLYTOTARGET,

//just used in scripting
	MTWEETYAI_SETHOMEPOS,
	MTWEETYAI_TURN,
	MTWEETYAI_TURNOPP,
	MTWEETYAI_START,
	MTWEETYAI_ANIM,
	MTWEETYAI_END,
	MTWEETYAI_PLAYSAMPLE,
	MTWEETYAI_STOPSAMPLE,
	MTWEETYAI_LOOP,
	MTWEETYAI_NEXTCAMERA,
	MTWEETYAI_COLLISIONOFF,
	MTWEETYAI_COLLISIONON,
	MTWEETYAI_RAISEMAGNET,
	MTWEETYAI_LOWERMAGNET,
	MTWEETYAI_LOWERSHIELD,
	MTWEETYAI_RAISESHIELD,
	MTWEETYAI_OPENDOOR,
	MTWEETYAI_SCALE,
	MTWEETYAI_DROPSHIELDFLICKER,
};

enum EMTweetyAISubState
{
	// CMD: transporter sub states
	TRANSPORTSTATE_APPEAR,
	TRANSPORTSTATE_DISAPPEAR,
	TRANSPORTSTATE_FINISHED,

	// CMD: flying substates
	TWEETYFLY_LIFTOFF,
	TWEETYFLY_CRUISE,
	TWEETYFLY_LAND,

	// CMD: rocket substates
	TWEETYROCKETS_PREPARE,
	TWEETYROCKETS_FIRE,
};

typedef struct TAG_MTWEETY_RUMBLE
{
	// CMD: rumble
	int			r_amplitude;
	float		r_attack, r_sustain, r_duration, r_delay, r_frequency, r_freqRateOfChange;
	EWaveform	r_waveform;
	// CMD: buzzer
	float		b_duration, b_delay, b_frequency, b_freqRateOfChange;
	EWaveform	b_waveform;
}MTWEETY_RUMBLE, _MTWEETYRUMBLE;

typedef struct TAG_MTWEETY_EVENT
{
	EMTweetyAIState		action;
	char				*targetInstance;
	float				time;
	MTWEETY_RUMBLE		*rumble;
	char				*sound;
} MTWEETY_EVENT, _MTWEETY_EVENT;

typedef int32 (*MTweetyAICallback)(struct TAG_MECHATWEETYINFO *mechaTweetyInfo);		//callback for buzzard ai
typedef int (*MTWeetyStateChangeCallback)(struct TAG_MECHATWEETYINFO *mechaTweetyInfo, EMTweetyAIState state, EMTweetyAIState lastState);	//called when aiState changes

typedef struct TAG_MECHATWEETYINFO
{
	TBVector			homePos;
	TBVector			targetPos;
	TBVector			tempVector;
	TBQuaternion		homeOrientation;
	TBQuaternion		targetOrientation;

	EMTweetyAIState		aiState;
	EMTweetyAIState		aiLastState;
	MTweetyAICallback	aiCallback;
	MTWeetyStateChangeCallback	stateChangeCallback;

	ACTORINSTANCE		*actorInstance;
	float				turnAmount;
	float				clock;
	uint32				flags;

	MTWEETY_EVENT		*currentScript;
	MTWEETY_EVENT		*lastScript;
	int32				scriptIndex;
	int32				scriptIndexLastFrame;

	ELECTRICITYATTACKINFO	*leftElectricAttack;
	ELECTRICITYATTACKINFO	*rightElectricAttack;
	int32					attachHandle;
	uint32					currentStage;

	uint32					hitsToNextStage;
	TBActorNodeInstance		*headNode;
	uint32					animCRC;
	uint32					aiSubState;

	struct
	{
		float				scale;
		float				targetScale;
		float				initialScale;
		float				finalScale;
		int32				scaleStage;
	}scaleInfo;

	REDSHIELDINFO			*shieldInfo;
	MAGNETSTREAK			*magnetStreak;
	MTweetyAttackPosition	attackPosition;
	int						rocketsRumbleHandle;

	int						genericRumbleHandle;
	int						magnetSoundHandle;
	int						flySoundHandle;
	int32					iPad;
}MECHATWEETYINFO, _MECHATWEETYINFO BALIGN16;

typedef struct TAG_MTWEETY_GENERICINFO
{
	TBVector				magnetNodes[3];									//[2] is [1] relative to [0]

	uint32					currentCamera;
	uint32					destinationCamera;
	struct TAG_CAMERAINFO	*cameraInfo;
	float					clock;

	float					magnetClock;
	float					magnetDistance;
	char					magnetsDestroyed[4];
	uint32					flags;

	uint32					openDoor;
	int						magnetSoundHandle;
	int32					iPad[2];
} MTWEETY_GENERICINFO, _MTWEETY_GENERICINFO BALIGN16;

extern struct TAG_ACTORINSTANCE		*globalMechaTweety;
extern MTWEETY_GENERICINFO			*mTweetyGenericInfo;

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMechaTweety
   Purpose : Create mecha tweety for final boss game
   Parameters : 
   Returns : 
   Info : Should be called after CreateActorInstance
*/

int CreateActorInstanceMechaTweety(struct TAG_ACTORINSTANCE *markerInstance);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyIdleCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyIdleCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyCrateAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyCrateAttackCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyElectricityAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyElectricityAttackCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyStageOneMoveCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyStageOneMoveCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int MechaTweetyStateChangeCallback(MECHATWEETYINFO *mechaTweetyInfo, EMTweetyAIState state, EMTweetyAIState lastState);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyDefendCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyDefendCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyGetCrateCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyGetCrateCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyTurnCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyTurnCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : BuzzardTakeOffCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyStunnedCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetySetHomePosCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetySetHomePosCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyNextCameraCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyNextCameraCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyTransportCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyTransportCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyPlayAnimCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyReturnTrueCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyMagnetChargeCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyFlyCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyOpenDoorCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyOpenDoorCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : MechaTweetyScaleCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyScaleCallback(MECHATWEETYINFO *mechaTweetyInfo);

/* --------------------------------------------------------------------------------
   Function : UpdateMechaTweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateMechaTweety(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateMechaTweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateMechaTweetyBossGameLogic(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void DrawMechaTweetySpecial(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : FreeMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void FreeMechaTweetySpecial(void);

/* --------------------------------------------------------------------------------
   Function : FreeMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateCrateCollisionWithMechaTweety(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMechaTweety
   Purpose : Create mecha tweety for final boss game
   Parameters : 
   Returns : 
   Info : Should be called after CreateActorInstance
*/

void InitialiseMTweetyGenericInfo(void);

/* --------------------------------------------------------------------------------
   Function : UpdateMTweetyGenericInfo
   Purpose : updates the camera mostly
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMTweetyGenericInfo(void);

/* --------------------------------------------------------------------------------
   Function : FreeMTweetyGenericInfo
   Purpose : frees all the gubbins that didn't seem to fit anywhere else
   Parameters : 
   Returns : 
   Info : 
*/

void FreeMTweetyGenericInfo(void);

#endif