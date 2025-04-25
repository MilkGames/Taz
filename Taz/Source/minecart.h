#ifndef __H_MINECART
#define __H_MINECART

#include "actors.h"
#include "camera.h"
#include "fade.h"
#include "animation.h"
#include "control.h"

#define MINECART_ACTIVE				(1<<0)
#define MINECART_RETURN				(1<<1)
#define MINECART_LEFTTURN			(1<<2)
#define MINECART_RIGHTTURN			(1<<3)
#define MINECART_CESTTOUT			(1<<4)		// CMD: that's it, at the end of the next segment your getting kicked
#define MINECART_MOVETOPOSITION		(1<<5)
#define MINECART_NOCONTROL			(1<<6)
#define MINECART_TUMBLE				(1<<7)
#define MINECART_DROP				(1<<8)
#define MINECART_BOING				(1<<9)
#define MINECART_DONOTHING			(1<<10)
#define MINECART_GONNADROP			(1<<11)		// CMD: cart will drop at next drop point
#define	MINECART_LEFTMISSING		(1<<12)
#define MINECART_RIGHTMISSING		(1<<13)
#define MINECART_LEFTOBSTACLE		(1<<14)
#define MINECART_RIGHTOBSTACLE		(1<<15)
#define MINECART_DROPLEFT			(1<<16)
#define MINECART_DROPRIGHT			(1<<17)
#define MINECART_ONFIRE				(1<<18)
#define MINECART_TRAGICSOUND		(1<<19)


#define MINECART_TILTBOUND			(2.0f)
#define MINECART_TILTMAXREBOUND		(6.0f)
#define MINECART_TILTREBOUNDRATE	(1.0f)
#define MINECART_SMOOTHRATE			(4.0f)
#define MINECART_LOSESTRENGTH		(0.1f)
#define MINECART_GAINSTRENGTH		(3.0f)
#define MINECART_STRENGTHTIME		(2.0f)
#define MINECART_MAXBOOSTTIME		(3.0f)
#define MINECART_MEXSPEEDFACTOR		(2.0f)


#define MINECART_ACCELERATION		(1.0f)
#define MINECART_CONTROLTILTRATE	(1.0f)
#define MINECART_RETURNTILTRATE		(4.0f)
#define MINECART_MAXTILTTIME		(1.0f)

#define MINECART_NUMSPARKVERTS		(10)

enum EBoostAnim
{
	MINECART_READYBOOST,
	MINECART_BOOST,
	MINECART_EXITBOOST,
	MINECART_NOBOOST,
	MINECART_JUMP,
	MINECART_BRAKE,
};

enum EDefaultPath
{
	MINECART_PATH_LEFT,
	MINECART_PATH_MIDDLE,
	MINECART_PATH_RIGHT,
};

typedef struct TAG_MINECARTSPARKS
{
	TBVector		offset;
	TBVector		position;

	TBTexture		*texture;
	int32			draw;
	int32			ipad[2];

	TBPrimVertex	verts[MINECART_NUMSPARKVERTS];
}MINECARTSPARKS, _MINECARTSPARKS BALIGN16;

typedef struct TAG_MINECARTINFO
{
	TBVector	cameraPosOffset;
	TBVector	cameraLookAtOffset;

	float		cameraPosSmooth;
	float		cameraLookAtSmooth;
	uint32		flags;
	uint32		landpointCRC;

	ANIMBLENDINFO	animBlendInfo;

	uint32		resetPointCRC;
	EDefaultPath		defaultPath;
	TBActorAnimSegment	*left;
	TBActorAnimSegment	*middle;

	TBActorAnimSegment	*right;

//control
	float		speedFactor;
	float		tilt;
	float		tiltTarget;

	float		tiltBound;
	float		tiltRebound;
	float		strength;
	float		strengthClock;

	int32		slowingDown;
	float		tiltClock;				// CMD: limits the amount of time that you can tilt for
	float		balancePoint;
	float		boostRemaining;

//sound
	int			soundNormal;
	int			soundBoost;
	int			soundSpark;
	int			iPad;

	ACTORINSTANCE	*dummyInstance;					// CMD: mine cart instance (the one you can see)
	ACTORINSTANCE	*characterOnCart;				// CMD: instance of character, riding cart
	EBoostAnim		boostAnimState;
	MINECARTSPARKS	*wheelSparks;
} MINECARTINFO, _MINECARTINFO BALIGN16;

typedef struct TAG_MINECARTDUMMYINFO
{
	TBVector		posLastFrame;
	TBVector		velocity;

	struct TAG_ACTORINSTANCE	*minecartInstance;
	int32						iPad[3];
}MINECARTDUMMYINFO, _MINECARTDUMMYINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateMineCart
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void CreateMineCart(ACTORINSTANCE *actorInstance, uint32 resetPointCRC);

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartCollision
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMineCartCollision(ACTORINSTANCE *collider, ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartCollision
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMineCart(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: AnimateMineCart
	Purpose		: Update the animation for an actorInstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void AnimateMineCart(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMineCartInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMineCartInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance);

/* -------------------------------------------------------------------------------
   Function : CharacterRespondToGhostTownRide
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToGhostTownRide(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance);

/* -------------------------------------------------------------------------------
   Function : CharacterRespondWestHubSevenMineCart
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondWestHubSevenMineCart(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance);

/* --------------------------------------------------------------------------------
   Function : CreateMineCartDummy
   Purpose : creates instance which is dummy layer for specialtype_minecart
   Parameters : 
   Returns : 
   Info : 
*/

int CreateMineCartDummy(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartDummyCollision
   Purpose : hides mine cart dummy, and calls respond to coll for minecart
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMineCartDummyCollision(ACTORINSTANCE *collider, ACTORINSTANCE *minecartDummy);

/* --------------------------------------------------------------------------------
   Function : UpdateMineCartAnimation
   Purpose : updates dummy minecart's, and Taz's animation
   Parameters : path actorInstance
   Returns : 
   Info : 
*/

void UpdateMineCartAnimation(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TumbleMineCart
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartTumble(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : TumbleMineCart
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

int TumbleMineCart(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : StartMineCartFire
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartFire(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : MineCartOnFire
   Purpose : Controls the update of burning minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

int MineCartOnFire(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : StartMineCartDrop
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartMissingRailAction(ACTORINSTANCE *minecart, EDefaultPath direction);

/* --------------------------------------------------------------------------------
   Function : DropMineCart
   Purpose : Controls the update of minecarts which are falling into deep chasms
   Parameters : 
   Returns : TRUE when drop has finished
   Info : 
*/

int DoMissingRailStuff(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : StartMineCartDrop
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartDrop(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : DropMineCart
   Purpose : Controls the update of minecarts which are falling into deep chasms
   Parameters : 
   Returns : TRUE when drop has finished
   Info : 
*/

int DropMineCart(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartAfterResetEvent(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartFadeDownCallback(FADE *fade);

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartAfterCompletion(ACTORINSTANCE *minecart);

/* --------------------------------------------------------------------------------
   Function : CreateMineCart
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMinecartDebugInfo(void);

/* --------------------------------------------------------------------------------
   Function : CreateMinecartSparks
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMinecartSparks(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawMinecartSparks
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMinecartSparks(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : TragicMinecartEventSounds
   Purpose : Clears any looping sounds, and plays an event sound
   Parameters : 
   Returns : 
   Info : 
*/

int TragicMinecartEventSounds(MINECARTINFO *info, char *sampleName);

#endif