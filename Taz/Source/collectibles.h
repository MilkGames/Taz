#ifndef __H_COLLECTIBLES
#define __H_COLLECTIBLES

#include "debris.h"
#include "actors.h"

// PP: in the cut-down consumer demo, there are only 50 sandwiches
// PP: normally there are 100
#ifdef CONSUMERDEMO
#define NUM_PICKUPS_EACH_LEVEL		50		// PP: THE NUMBER OF PICKUPS ON THE CONSUMER DEMO LEVEL
#else// PP: not demo
#define NUM_PICKUPS_EACH_LEVEL		100		// PP: THE NUMBER OF PICKUPS ON EACH LEVEL NORMALLY
#endif// PP: not demo

#define MAX_COLLECTIBLES			(200)
#define SCREENTINT_FLASHTIME		(5.0f)
#define POWERUP_GLOW_RADIUS			(METERTOUNIT(0.25f))
#define	DE_POWERUP_GLOW_RADIUS		(METERTOUNIT(0.5f))
#define POWERUP_GLOW_ALPHA			(60)
#define POWERUP_GLOW_LIFE			(3.0f)
#define POWERUP_GLOW_SPEED			(METERTOUNIT(0.15f))
#define POWERUP_GLOW_BALL_SIZE		(METERTOUNIT(0.1f))
#define POWERUP_GLOW_BALL_VERT		(METERTOUNIT(0.75f))
#define POWERUP_GLOW_BALL_TIME		(0.04f)
#define POWERUP_RESPAWN_TIME		(15.0f)

#define TWINKLE_MIN_RADIUS			(METERTOUNIT(0.05f))
#define TWINKLE_RAD_FROM_POINT		(METERTOUNIT(0.25f))
#define TWINKLE_ALPHA				(60)
#define	TWINKLE_HALF_LIFE			(0.5f)

#define SHOWER_MIN_VEL				(125.0f)
#define SHOWER_INIT_ALPHA			(120.0f)

#define COLLECTIBLES_SCALE_RATE		(540.0f)
#define COLLECTIBLES_AMP_CONST		(METERTOUNIT(4.0f))
#define COLLECTIBLES_ROT_SPEED		(RAD(180.0f))
#define TAZ_ON_MINECART_COLL_RADIUS	(METERTOUNIT(0.2f))

#include "colour.h"

enum ECollectibleType
{
	COLLECTIBLETYPE_NONE,
	COLLECTIBLETYPE_FOOD,
	COLLECTIBLETYPE_BIGFOOD,
	COLLECTIBLETYPE_STRANGEFOOD,
	COLLECTIBLETYPE_TAZCAM,
	COLLECTIBLETYPE_PICKUP,
	COLLECTIBLETYPE_BURPCAN,
	COLLECTIBLETYPE_TESTTUBE,
	COLLECTIBLETYPE_HICCUP,
	COLLECTIBLETYPE_BUBBLEGUM,
	COLLECTIBLETYPE_CHILLIPEPPER,
	COLLECTIBLETYPE_SECURITYBOX,
	COLLECTIBLETYPE_SPINPAD,
	COLLECTIBLETYPE_DE_EXTRATIME,
	COLLECTIBLETYPE_DE_DOUBLECRATE,
	COLLECTIBLETYPE_DE_SLOMO,
	COLLECTIBLETYPE_SECRETITEM,
	ENDOFCOLLECTIBLETYPES,
};

enum EScreenTintState
{
	SCREENTINT_NONE,
	SCREENTINT_STEADY,
	SCREENTINT_FLASHING,
};

enum EScreenScoreType
{
	SCREENSCORE_REWARD,
	SCREENSCORE_PENALTY,
	SCREENSCORE_SANDWICHES,
};

#define COLLECTIBLEFLAG_NEW				(1<<0)
#define COLLECTIBLEFLAG_DONTDRAW		(1<<1)
#define COLLECTIBLEFLAG_DISABLED		(1<<2)
#define COLLECTIBLEFLAG_POWERUP			(1<<3)
#define COLLECTIBLEFLAG_RESPAWN			(1<<4)
#define COLLECTIBLEFLAG_INACTIVE		(1<<5)
#define COLLECTIBLEFLAG_DONTREMOVE		(1<<6)

enum
{
	COLLECTIBLE_OFF,
	COLLECTIBLE_SHRINK,
	COLLECTIBLE_ENLARGE,
	COLLECTIBLE_ON,
};

typedef struct TAG_COLLECTIBLE
{
	TBVector				position;		

	TBVector				velocity;

	TBActorInstance		*actorInstance;		// if collectible is an actor / null if not
	TBTexture			*texture;			// if collectible is a sprite / null if not
	float				life;				// life in seconds - 0 if infinite
	uint32				flags;

	int32				radius;				// radius of object for collection purposes
	struct TAG_SHADOW	*shadow;			// shadow
	char				type;				// type of collectible
	char				state;				// current state of collectible
	float				distanceFromCam;	// distance from camera

	COLOUR				colour;				// TP: colour to apply to texture if sprite collectible

	struct TAG_COLLECTIBLEPOWERUP	*powerUpInfo;
	uint32				crc;				// TP: crc of collectible obe name
	float				counter;			// TP: time counter used by the COLLECTIBLETYPE_PICKUP for colour cycling
	float				textureRot;			// TP: z rotation of texture

	float				pulse;
	float				twinkleClock;
	float				twinkleDeadTime;
	int					ident;				// TP: optional identifier

	float				phaseAngle;
	float				rotAngle;
	float				glowAlpha;
	int32				iPad;
} COLLECTIBLE, _COLLECTIBLE BALIGN16;

typedef struct TAG_COLLECTIBLEPOWERUP
{
	int					glowColour[4];

	float				glowSize;
	float				glowTilt;
	float				clock;
	float				respawnClock;

	int					respawn;			// CMD: does the power up respawn
	int					iPad[3];
} COLLECTIBLEPOWERUP, _COLLECTIBLEPOWERUP BALIGN16;

typedef struct TAG_PICKUPINFO
{
	int					soundHandle;		// TP: Handle for the siren sound used by the pickups
	float				disableTime;		// TP: if not zero this is the time
	float				tintVal;			// TP: tint value used to set the red screen overlay
	uint32				tintState;			// CMD: current state of the tint

	float				flashingClock;		// CMD: time in flashing state
	int					iPad[3];
} PICKUPINFO, _PICKUPINFO BALIGN16;


// CMD: list of screen scores (eg text from snadwiches) from collectibles currently showing at a screen near you
#define		SCREENSCORE_TIME		(1.0f)

#define		SCREENSCORE_OUTLINE		(1<<0)

typedef struct TAG_SCREENSCOREMEMBER
{
	TBVector				velocity;
	TBVector				position;
	TBVector				targetPos;
	TBVector				offset;

	char					string[16];

	int						r, g, b, a;

	float					alphaRate;
	TBPrintFontCallback		callback;
	EScreenScoreType		type;
	float					angle;

	float					scale;
	uint32					flags;
	struct TAG_SCREENSCOREMEMBER		*next;
	struct TAG_SCREENSCOREMEMBER		*prev;
} SCREENSCOREMEMBER, _SCREENSCOREMEMBER BALIGN16;

typedef struct TAG_SCREENSCORELIST
{
	SCREENSCOREMEMBER		head;

	uint32					numEntries;
	uint32					pad[3];
} SCREENSCORELIST, _SCREENSCORELIST BALIGN16;

//extern	char	numPickupsRegen;
extern	TBExtraSystem	*collectibleSystem;

/* --------------------------------------------------------------------------------
   Function : InitBarrierSystem
   Purpose : Call this function to init the counters and siren sound for the taz cam shutdown thingys
   Parameters : none
   Returns : 
   Info : 
*/
void InitBarrierSystem(void);

/* --------------------------------------------------------------------------------
   Function: ShutdownBarrierSystem
   Purpose : This function clears anything left behind by the taz cam shutdown stuff
   Parameters : none
   Returns : 
   Info : 
*/
void ShutdownBarrierSystem(void);

/* --------------------------------------------------------------------------------
   Function : InitCollectibleSystem
   Purpose : call this function to create the collectible extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCollectibleSystem(void);

/* --------------------------------------------------------------------------------
   Function : CreateCollectibleActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create collectible from
   Returns : TRUE /FALSE
   Info : 
*/
int CreateCollectibleActor(char *actorName, TBVector position, float life, int radius, char type, int respawn=TRUE,int ident=0);

/* --------------------------------------------------------------------------------
   Function : CreateCollectibleTexture
   Purpose : Call this function to create a collectilble texture
   Parameters : actor to create collectible from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/
int CreateCollectibleTexture(char *textureName, TBVector position, float life, int radius, char type, int colour[3],int ident=0);

/* --------------------------------------------------------------------------------
   Function : CollectibleCreateCallback
   Purpose : This function is called when a collectible is created
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraDeleteCallback
   Purpose : This function is called when a collectible is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CollectibleExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : UpdateCollectible
   Purpose : This prototype seemed to have gotton lost, and found its way into the .cpp file
   Parameters : 
   Returns : 
   Info : 
*/

int UpdateCollectible(COLLECTIBLE *data);

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraUpdateCallback
   Purpose : This function is called each frame to update the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraUpdateCallback
   Purpose : This function is called each frame to render the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : ProcessItemCollected
   Purpose : This function is called when an item is collected
   Parameters : actor collecting the item, item type
   Returns : 
   Info : 
*/
void ProcessItemCollected(ACTORINSTANCE *actorInstance, COLLECTIBLE *);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCollectibleFoodScaleAndOrientation
	Purpose 	: see above
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateCollectibleFoodScaleAndOrientation(COLLECTIBLE *food, float angularVelocity);

/*  --------------------------------------------------------------------------------
	Function	: ShutdownTazCameras
	Purpose		: Used when the player has destroyed enough pickups
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: Starts the 20 second countdown before taz cams re-activate
*/
void ShutdownTazCameras(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function: UpdateBarrierSystem
   Purpose : Controls the barriers, update the timers and the screen tint
   Parameters : none
   Returns : 
   Info : 
*/
int UpdateBarrierSystem(void);


/* --------------------------------------------------------------------------------
   Function: CreatePowerUpBubbles
   Purpose : Creates bubbles which eminate from powerups
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreatePowerUpBubbles(struct TAG_COLLECTIBLE *data);

/* --------------------------------------------------------------------------------
   Function: CreateCollectibleTwinkles
   Purpose : Creates twinkles around collectibles
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreateCollectibleTwinkles(struct TAG_COLLECTIBLE *data, uint32 flags=0);

/* --------------------------------------------------------------------------------
   Function: UpdateTwinkles
   Purpose : Creates twinkles around collectibles
   Parameters : debris structure
   Returns : 
   Info : 
*/

int32 UpdateTwinkles(BUBBLESEXTRA *data);

/* --------------------------------------------------------------------------------
   Function: CreateCollectibleShower
   Purpose : Creates shower for when collectibles are collected
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreateCollectibleShower(struct TAG_COLLECTIBLE *data, float groundCoord);

void CreateCollectibleShowerAtPosition(TBVector position, float groundCoord);

/* --------------------------------------------------------------------------------
   Function: UpdateCollectiblesShower
   Purpose : updates shower produced when collectibles are collected
   Parameters : debris structure
   Returns : 
   Info : 
*/

int32 UpdateCollectiblesShower(BUBBLESEXTRA *data);

void myStateChangeBubble(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function: ClearCollectibles
   Purpose : Clears all collectables to respawn
   Parameters : 
   Returns : 
   Info : 
*/
void ClearCollectibles(void);

/*  --------------------------------------------------------------------------------
	Function	: HasPickupBeenCollected
	Purpose		: Like it sounds
	Parameters	: number of pickup
	Returns		: true/false
	Info		: 
*/
bool HasPickupBeenCollected(int	number);

void EatBadFoodCallback(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ActivateSecretItem
	Purpose		: Used to make the secret item visable and active
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ActivateSecretItem(void);

/*  --------------------------------------------------------------------------------
	Function	: DeactivateSecretItem
	Purpose		: Used to deactivate and hide the secret item
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DeactivateSecretItem(void);

/* --------------------------------------------------------------------------------
   Function : InitScreenScoreList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitScreenScoreList(void);

/* --------------------------------------------------------------------------------
   Function : CreateScreenScoreMember
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateScreenScoreMember(TBVector position, char *string, EScreenScoreType type);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromFallingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void RemoveScreenScoreMember(SCREENSCOREMEMBER *remPtr);

/* --------------------------------------------------------------------------------
   Function : UpdateScreenScoreList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateScreenScoreList(void);

/* --------------------------------------------------------------------------------
   Function : DrawScreenScoreList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void DrawScreenScoreList(void);

/* --------------------------------------------------------------------------------
   Function : FreeScreenScoreList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeScreenScoreList(void);

/* --------------------------------------------------------------------------------
   Function : ScreenScoreRotateToTargetCallback
   Purpose : spins the font towards a target coordinate
   Parameters : 
   Returns : 
   Info : 
*/

int ScreenScoreRotateToTargetCallback(EBFontCallbackReason reason, TBFontCallbackInfo *info);

/*  --------------------------------------------------------------------------------
	Function	: myStateChangeBubble
	Purpose		: Callback used when changing into the bubblegum state
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void myStateChangeBubble(ACTORINSTANCE *actorInstance);

#endif