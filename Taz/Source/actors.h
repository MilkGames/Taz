#ifndef __H_ACTORS
#define __H_ACTORS

#include "ShadowExtra.h"			// PP: Useful shadow stuff that needs to be included when shadow.h can't be

// PP: define MORE_CARTOON_RENDERING to do more outlining and cel-shading than the PS2 version does.
// PP: TODO?: have this as an optional thing on PC version?
#if(BPLATFORM == XBOX)
#define MORE_CARTOON_RENDERING				true
#elif(BPLATFORM==GAMECUBE)
#define MORE_CARTOON_RENDERING				true
#else
#define MORE_CARTOON_RENDERING				false
#endif// PP: platform

// PP: enable cartoon render mode if MORE_CARTOON_RENDERING is true, otherwise enable line render mode
#define ENABLE_LINE_OR_CARTOON_RENDER_MODE(_actorInstance) \
		if(MORE_CARTOON_RENDERING) \
		{ \
			EnableActorCartoonRenderMode(_actorInstance); \
		} \
		else \
		{ \
			EnableActorLineRenderMode(_actorInstance); \
		}

#define ACTORTYPE_ACTOR (1<<0)
#define ACTORTYPE_SET	(1<<1)
#define ACTOR_MALLOCED	(1<<2)		// set if the actor was not part of a loaded package
#define ACTOR_PERMANENT (1<<3)

// TP: 32 bits available
#define ACTORFLAG_NOCOLLISION				(1<<0)
#define ACTORFLAG_DONTDRAW					(1<<1)
#define ACTORFLAG_DESTROYNEXTUPDATE			(1<<2)
#define ACTORFLAG_DEBRISACTOR				(1<<3)
#define ACTORFLAG_ALWAYSCALLPRECOLLCALLBACK	(1<<4)
#define ACTORFLAG_DELETEAFTERFADEOUT		(1<<5)
#define ACTORFLAG_ALLOWUNSQUASHMOVE			(1<<6)
#define ACTORFLAG_CUSTOMDRAW				(1<<7)
#define ACTORFLAG_DONTFADE					(1<<8)
#define ACTORFLAG_NOWORLDCOLLISION			(1<<9)
#define ACTORFLAG_FADEAFTERANIM				(1<<10)
#define ACTORFLAG_NOVELOCITY				(1<<11)
#define ACTORFLAG_HASANIMATED				(1<<12)
#define ACTORFLAG_UPDATECOLLAFTERANIM		(1<<13)
#define ACTORFLAG_DONTCHECKBOUNDINGBOX		(1<<14)
#define ACTORFLAG_DONTFOG					(1<<15)
#define ACTORFLAG_ALWAYSPREP				(1<<16)
#define ACTORFLAG_INSHADE					(1<<17)			// PP: the actorinstance is in shade (cel-shade it entirely in low-light)// PP: added for the Zoo Boss

// TP: flags to control what is free'd by FreeActorInstance
#define DONTFREE_ACTORINSTANCE				(1<<0)
#define DONTFREE_DESTRUCTIBLEINFO			(1<<1)

// TP: Draw flags for passing into DrawActorInstance and DrawActorInstance2
#define DRAWACTORFLAG_DONTCLEARONSCREEN		(1<<0)

enum ESpecialType
{
	SPECIALTYPE_PROJECTILE,
	SPECIALTYPE_PROJECTILESLIDE,
	SPECIALTYPE_FALLINGOBJECT,
	SPECIALTYPE_DIVINGBOARD,
	SPECIALTYPE_TRAMAMPOLINE,
	SPECIALTYPE_STEPPINGSTONE,
	SPECIALTYPE_TILTINGOBJECT,
	SPECIALTYPE_SEESAWOBJECT,
	SPECIALTYPE_SWINGINGOBJECT,
	SPECIALTYPE_WATERYOBJECT,
	SPECIALTYPE_DESTRUCTIBLE,
	SPECIALTYPE_MAILBOX,
	SPECIALTYPE_WANTEDPOSTER,
	SPECIALTYPE_PNEUMATICPLUNGER,
	SPECIALTYPE_PATH,
	SPECIALTYPE_PLATFORM,
	SPECIALTYPE_RESETPOINT,
	SPECIALTYPE_TRIGGER,
	SPECIALTYPE_HIGHVOLTAGE,
	SPECIALTYPE_EFFECT,
	SPECIALTYPE_CAMERACASE,
	SPECIALTYPE_PRECIPITATION,
	SPECIALTYPE_ELECTRICITY,
	SPECIALTYPE_ZOOKEEPER,
	SPECIALTYPE_FIRE,
	SPECIALTYPE_TRANSPORTER,
	SPECIALTYPE_ICE,
	SPECIALTYPE_SPEEDBOOST,
	SPECIALTYPE_SLIDE,
	SPECIALTYPE_RANDOMFOOD,
	SPECIALTYPE_ESCALATOR,
	SPECIALTYPE_VEHICLE,
	SPECIALTYPE_DOOR,
	SPECIALTYPE_SCENIC,
	SPECIALTYPE_PHONEBOX,
	SPECIALTYPE_WATERFALL,
	SPECIALTYPE_LASER,
	SPECIALTYPE_FOUNTAIN,
	SPECIALTYPE_SHATTER,
	SPECIALTYPE_TARBUBBLE,
	SPECIALTYPE_ENEMY,
	SPECIALTYPE_LIGHTNING,
	SPECIALTYPE_CHECKPOINT,
	SPECIALTYPE_BUMPER,
	SPECIALTYPE_MONITOR,							// PP: special type for computer monitors
	SPECIALTYPE_TARGET,
	SPECIALTYPE_SAFEPOINT,							// CMD: safepoint for Taz while stats are showing
	SPECIALTYPE_MAGNET,
	SPECIALTYPE_FLOCKING,
	SPECIALTYPE_BUZZARD,
	SPECIALTYPE_FECAMERA,							// CMD: front end camera
	SPECIALTYPE_MAPPOINT,
	SPECIALTYPE_MIRROR,
	SPECIALTYPE_LAVA,
	SPECIALTYPE_LAVASPIT,
	SPECIALTYPE_SPLAT,
	SPECIALTYPE_DEATH,
	SPECIALTYPE_HAAR,
	SPECIALTYPE_MUSICBOX,
	SPECIALTYPE_MINECART,							// CMD: just seem to simpler keeping minecart separate from other vehicles
	SPECIALTYPE_MINECARTDUMMY,
	SPECIALTYPE_PORTALWORLD,
	SPECIALTYPE_FLAMETHROWA,
	SPECIALTYPE_SECURITYBOX,
	SPECIALTYPE_MECHATWEETY,
	SPECIALTYPE_TAZBOSSITEM,
	SPECIALTYPE_CLIPPLANE,							// CMD: 
	SPECIALTYPE_ROCKET,
	SPECIALTYPE_XDOOR,								// TP: special type for the planet X doors
	SPECIALTYPE_HALO,
	SPECIALTYPE_ZAPPER,
	SPECIALTYPE_SPAWNCRATE,
	SPECIALTYPE_POLICEBOX,
	SPECIALTYPE_AIRCON,
	SPECIALTYPE_TUMBLEWEED,
	SPECIALTYPE_ARROW,
	SPECIALTYPE_MIRRORBALL,
	SPECIALTYPE_WATERRIPPLE,
#if BPLATFORM==GAMECUBE
	SPECIALTYPE_REVERB,
#endif
	// TP: Make sure you update the string table in Actors.cpp

	NUM_SPECIAL_TYPES								// PP: number of special types
};

extern char *specialTypeNames[];

#define ACTORLISTNAMELENGTH		(32)

typedef struct TAG_ACTORLISTENTRY
{
	struct TAG_ACTORLISTENTRY	*next,*prev;
	TBActor						*actor;
	char						name[ACTORLISTNAMELENGTH];			//for debug purpose only, not used in search
	uint32						crc;
	uint32						type;				// specific information about actor type
} ACTORLISTENTRY;

typedef struct TAG_ACTORLIST
{
	ACTORLISTENTRY	head;
	int				numEntries;
} ACTORLIST;

extern ACTORLIST actorList;

typedef int (*RespondToCollision)(struct TAG_ACTORINSTANCE *collider,struct TAG_ACTORINSTANCE *object);
typedef int (*PreRespondToCollision)(struct TAG_ACTORINSTANCE *collider,struct TAG_ACTORINSTANCE *object, struct TAG_PRECOLLINFO *preCollInfo);
typedef int (*OutsideBoundingBox)(struct TAG_ACTORINSTANCE *actorInstance,struct TAG_ACTORINSTANCE *object);

typedef struct TAG_ACTORINSTANCE
{
	TBActorInstance					actorInstance;
//
	struct TAG_ACTORINSTANCE		*next,*prev;
	uint32							instanceCRC;
	uint32							actorCRC;
//
	char							instanceName[32];	//for debug purpose only, not used in search
	char							actorName[32];		//for debug purpose only, not used in search
//
	struct TAG_ACTOR_BEHAVIOUR		*actorBehaviour;	//structure for all physics and behaviour properties
	struct TAG_ACTOR_ANIMATION		*actorAnimation;	//structure for all animation information
	struct TAG_ACTOR_STATUS			*actorStatus;
	struct TAG_CHARACTERINFO		*characterInfo;		//if a character this stores all character info
//
	struct TAG_PATHINFO				*pathInfo;			//structure for all path following information
	struct TAG_SHADOW				*actorShadow;		//shadow if any
	struct TAG_GENERICAI			*genericAI;			//generic ai if any	
	RespondToCollision				RespondToCollisionRoutine; // routine to call upon collision
//
	struct TAG_ACTORINSTANCELIST	*instanceList;		// what instance list am I assigned to
	short							alpha;				// alpha value requested
	short							currentAlpha;		// current alpha value of object including fade out etc.(DONT USE)
	union {
		struct TAG_PROJECTILEINFO			*projectileInfo;
		struct TAG_FALLINGOBJECTINFO		*fallingObjectInfo;
		struct TAG_SPRINGYOBJECTINFO		*springyObjectInfo;
		struct TAG_STEPPINGSTONEOBJECTINFO	*steppingStoneObjectInfo;
		struct TAG_TILTINGOBJECTINFO		*tiltingObjectInfo;
		struct TAG_SWINGINGOBJECTINFO		*swingingObjectInfo;
		struct TAG_WATERYOBJECTINFO			*wateryObjectInfo;
		struct TAG_DESTRUCTIBLEOBJECTINFO	*destructibleObjectInfo;
		struct TAG_MAILBOXINFO				*mailboxInfo;
		struct TAG_WANTEDPOSTERINFO			*wantedPosterInfo;
		struct TAG_PNEUMATICPLUNGERINFO		*pneumaticPlungerInfo;
		struct TAG_TRIGGEROBJECTINFO		*triggerObjectInfo;
		struct TAG_HIGHVOLTAGEOBJECTINFO	*highVoltageObjectInfo;
		struct TAG_ELECTRICITYINFO			*electricityInfo;
		struct TAG_FIREINFO					*fireInfo;
		struct TAG_TRANSPORTERINFO			*transporterInfo;
		struct TAG_SPEEDBOOSTINFO			*speedboostInfo;
		struct TAG_ESCALATORINFO			*escalatorInfo;
		struct TAG_VEHICLEINFO				*vehicleInfo;
		struct TAG_DOORINFO					*doorInfo;
		struct TAG_SCENICINFO				*scenicInfo;
		struct TAG_PHONEBOXINFO				*phoneBoxInfo;
		struct TAG_WATERFALLINFO			*waterfallInfo;
		struct TAG_LASERINFO				*laserInfo;
		struct TAG_FOUNTAININFO				*fountainInfo;
		struct TAG_SHATTERINFO				*shatterInfo;
		struct TAG_TARBUBBLEINFO			*tarBubbleInfo;
		struct TAG_LIGHTNINGINFO			*lightningInfo;
		struct TAG_BUMPERINFO				*bumperInfo;
		struct TAG_MONITOREFFECTINFO		*monitorInfo;						// PP: special-type info for computer monitor instances
		struct TAG_MAGNETINFO				*magnetInfo;
		struct TAG_FLOCKINGINFO				*flockingInfo;
		struct TAG_BUZZARDINFO				*buzzardInfo;
		struct TAG_LAVAOBJECTINFO			*lavaInfo;
		struct TAG_LAVASPITINFO				*lavaSpitInfo;
		struct TAG_DEATHOBJECTINFO			*deathObjectInfo;
		struct TAG_HAARINFO					*haarInfo;
		struct TAG_MINECARTINFO				*mineCartInfo;
		struct TAG_MINECARTDUMMYINFO		*mineCartDummyInfo;
		struct TAG_MUSICBOXINFO				*musicBoxInfo;
		struct TAG_FLAMETHROWAINFO			*flamethrowaInfo;
		struct TAG_MECHATWEETYINFO			*mechaTweetyInfo;
		struct TAG_TAZBOSSITEMINFO			*tazBossItemInfo;
		struct TAG_ROCKETINFO				*rocketInfo;
		struct TAG_XDOORINFO				*xdoorInfo;
		struct TAG_HALOINFO					*haloInfo;
		struct TAG_ZAPPERINFO				*zapperInfo;
		struct TAG_AIRCONUNITINFO			*airconunitInfo;
		struct TAG_TUMBLEWEEDINFO			*tumbleweedInfo;
		struct TAG_SAFARIARROWINFO			*arrowInfo;
		struct TAG_MIRRORBALLINFO			*mirrorballInfo;
		struct TAG_WATERRINGOBJECTINFO		*waterRingInfo;
#if BPLATFORM==GAMECUBE		// IH: I just want to store one int of data - so why make an info block?
		int									soundEffectType;
#endif
		void								*specialType;
	};
	PreRespondToCollision				PreRespondToCollisionRoutine;	// routine to call upon collision
//
	float							fadeSpeed;						// amount to change alpha per second
	float							lineThickness;					//temp
	int								flags;							// actor flags
	ESpecialType					special;						// what special type is it
//
	OutsideBoundingBox				outsideBoundingBox;				//routine to call when instance exits bb of object which it has stopped colliding with
	int								soundHandle;					// TP: handle for any looped sound assigned to this object, one at a time please
	TBActorNodeInstance				*soundNodeInstance;				// TP: Pointer to the node on this actor that the sound should be played on
	short							pad;							// TP: Feel free to use these
	char							onScreen;						// TP: TRUE if object is currently onscreen
	char							playerNo;						// TP: player number or -1
} ACTORINSTANCE;

#define ACTORINSTLISTFLAG_ANIMATED (1<<0)

typedef struct TAG_ACTORINSTANCELIST
{
	ACTORINSTANCE			head;
	int						numEntries;
	int						flags;
} ACTORINSTANCELIST;


/* --------------------------------------------------------------------------------
   Function : InitActorList
   Purpose : Intialise actor list
   Parameters : 
   Returns : 
   Info : Initialise list for loaded actors
*/
void InitActorList(void);

/* --------------------------------------------------------------------------------
   Function : InitActorStructures
   Purpose : Intialise actor instance structures
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitActorStructures(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SetInstanceListParms
   Purpose : Set the instance list flags
   Parameters : ACTORINSTANCELIST, int flags
   Returns : 
   Info : 
*/
void SetInstanceListParms(ACTORINSTANCELIST *actorInstanceList,int flags);

/* --------------------------------------------------------------------------------
   Function : AddActorToActorList
   Purpose : Add a new actor to the actor list
   Parameters : TBActor, identifierName
   Returns : 
   Info : Add an actor to the actorlist and associates and identifier name to it for later use
*/
void AddActorToActorList(TBActor *actor,char *name,uint32 type);

/* --------------------------------------------------------------------------------
   Function : AddActorToActorListByCRC
   Purpose : Add a new actor to the actor list
   Parameters : TBActor, identifierName
   Returns : 
   Info : Add an actor to the actorlist and associates and identifier name to it for later use
*/
void AddActorToActorListByCRC(TBActor *actor,uint32 crc,uint32 type);

/* --------------------------------------------------------------------------------
   Function : SetActorRenderMode
   Purpose : Set current render mode
   Parameters : actorInstance, mode (ACTORFLAGS_CARTOONRENDER etc.)
   Returns : 
   Info : The actual render mode used might differ base on distance from camera.
*/
void SetActorRenderMode(ACTORINSTANCE *actorInstance, uint32 mode);

/* --------------------------------------------------------------------------------
   Function : EnableActorLineRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorLineRenderMode(TBActorInstance *actorInstance, TBActRenderMode_Outlined *parms = NULL);

/* --------------------------------------------------------------------------------
   Function : EnableActorLineOnlyRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorLineOnlyRenderMode(TBActorInstance *actorInstance, TBActRenderMode_Outlined *parms = NULL);

/* --------------------------------------------------------------------------------
   Function : SubActorFromActorList
   Purpose : Removes and actor from the actor list
   Parameters : identifierName
   Returns : pointer the the TBActor that has been removed from the list or NULL for could'nt find actor
   Info : Remove and actor from the actor list
*/
TBActor *SubActorFromActorList(char *name);

/* --------------------------------------------------------------------------------
   Function : LoadActor
   Purpose : Loads an actor from a package file and adds it to the actorList
   Parameters : file name, package
   Returns : TRUE, FALSE
   Info : 
*/
int LoadActor(char *actorName, char *packageName, int flags=0);

/* --------------------------------------------------------------------------------
   Function : LoadActor
   Purpose : Loads an actor from a package file and adds it to the actorList
   Parameters : file name, package, package index or NULL
   Returns : TRUE, FALSE
   Info : 
*/
int LoadActor(char *actorName, char *packageName, TBPackageIndex *pakIndex);

/* --------------------------------------------------------------------------------
   Function : LoadSet
   Purpose : Loads an set from a package file and adds it to the actorList
   Parameters : file name, package
   Returns : TRUE, FALSE
   Info : 
*/
int LoadSet(char *actorName, char *packageName);

/* --------------------------------------------------------------------------------
   Function : FreeActor
   Purpose : Removes an actor from the actor list and free the memory allocated completely
   Parameters : file name
   Returns : TRUE, FALSE
   Info : 
*/
int FreeActor(char *actorName);

/* --------------------------------------------------------------------------------
   Function : FreeAllActors
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllActors(void);

/* --------------------------------------------------------------------------------
   Function : FreeAllPermanentActors
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPermanentActors(void);

/* --------------------------------------------------------------------------------
   Function : FindActorInstanceInInstanceList
   Purpose : Locates an actor instance in by name
   Parameters : instance name, or crc, instance list
   Returns : ACTORINSTANCE or NULL
   Info : 
*/
ACTORINSTANCE *FindActorInstanceInInstanceList(char *name, uint32 crc, ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : FindActorInActorList
   Purpose : Locates an actor in the actorList by name
   Parameters : actor name
   Returns : TBActor, or NULL
   Info : 
*/
TBActor *FindActorInActorList(char *name);

/* --------------------------------------------------------------------------------
   Function : FindActorInActorList
   Purpose : Locates an actor in the actorList by name
   Parameters : actor name
   Returns : TBActor, or NULL
   Info : 
*/
TBActor *FindActorInActorList(uint32 crc);


/* --------------------------------------------------------------------------------
   Function : InitActorInstanceList
   Purpose : Intialise actor instance list
   Parameters : ACTORINSTANCELIST
   Returns : 
   Info : Initialise list for actor instances
*/
void InitActorInstanceList(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstance
   Purpose : Creates,initialises and adds an actor instance to an actor instancelist
   Parameters : actorInstanceList, actor name, instance name
   Returns : pointer to the actor instance entry created.
   Info : 
*/
ACTORINSTANCE *CreateActorInstance(ACTORINSTANCELIST *actorInstanceList,char *actorName,char *instanceName, TBVector position=NULL, TBVector scale=NULL, TBQuaternion orientation=NULL,short flags = 0);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceFromActorPtr
   Purpose : Creates,initialises and adds an actor instance to an actor instancelist
   Parameters : actorInstanceList, actor name, instance name
   Returns : pointer to the actor instance entry created.
   Info : 
*/
ACTORINSTANCE *CreateActorInstanceFromActorPtr(ACTORINSTANCELIST *actorInstanceList,TBActor *actorPtr,uint32 actorCrc,char *instanceName);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBehaviour
   Purpose : Creates and initialises the physics part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceBehaviour(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAnimation
   Purpose : Creates and initialises the animation part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceAnimation(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceStatus
   Purpose : Creates and initialises the status part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceStatus(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceShadow
   Purpose : Creates and initialises the shadow part of an actorInstance
   Parameters : ACTORINSTANCE, shadowRadius, shadowHeight, 
				(opt/default) max radius of object for shadow map rendering,
				(opt/DOWNWARD) shadow type (SHADOWTYPE_)
   Returns : 
   Info : // PP: added optional type parameter to cater for special occasions like the Zoo boss sunset
*/

// PP: default radius of an object casting a shadow map.  NOTE this is currently a radius about the object's origin!
// PP: **** THIS IS DELIBERATELY VERY SMALL SO THAT YOU DON'T ACCIDENTALLY CREATE OVER-BIG (SLOW) SHADOWS ON LITTLE THINGS AND NOT KNOW ABOUT IT.
// PP: **** YOU SHOULD DEFINE A SHADOW MAP RADIUS FOR ANY CHARACTER/OBJECT THAT NEEDS ONE, DON'T RELY ON THIS DEFAULT.
#define SHADOWMAP_DEFAULT_OCCLUDER_RADIUS		125.0f

int CreateActorInstanceShadow(ACTORINSTANCE	*actorInstance,
							  float			shadowRadius,
							  float			shadowHeight,
							  const float	shadowMapRadius	=SHADOWMAP_DEFAULT_OCCLUDER_RADIUS,
							  ShadowType	shadowType		=SHADOW_DEFAULT_TYPE);


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceCharacterInfo
   Purpose : Creates and initialises the character info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceCharacterInfo(ACTORINSTANCE *actorInstance, uint32 createFlags=0);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstancePathInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceGenericAI
   Purpose : Creates and initialises the ai part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : TRUE, FALSE
*/
int CreateActorInstanceGenericAI(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/* --------------------------------------------------------------------------------
   Function : FreeActorInstanceByName
   Purpose : Free an actor instance using the instance name
   Parameters : actorInstanceList, instance name
   Returns : TRUE,FALSE
   Info : 
*/
int FreeActorInstanceByName(ACTORINSTANCELIST *actorInstanceList,char *instanceName);

/* --------------------------------------------------------------------------------
   Function : FreeActorInstance
   Purpose : Free an actor instance using the actor instance
   Parameters : actorInstanceEntry
   Returns : TRUE,FALSE
   Info : 
*/
int FreeActorInstance(ACTORINSTANCE *actorInstanceEntry, int dontFreeFlags = 0);

/* --------------------------------------------------------------------------------
   Function : FreeDestructibleInfo
   Purpose : Frees all the memory used by the destructibleObjectInfo structure
   Parameters : DESTRUCTIBLEOBJECTINFO
   Returns : 
   Info : 
*/
void FreeDestructibleInfo(struct TAG_DESTRUCTIBLEOBJECTINFO *destructibleObjectInfo);

/* --------------------------------------------------------------------------------
   Function : FreeActorInstanceMemory
   Purpose : Frees all the memory assigned to an actor instance
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/
void FreeActorInstanceMemory(ACTORINSTANCE *ptr, int dontFreeFlags = 0);

/* --------------------------------------------------------------------------------
   Function : FreeAllActorInstances
   Purpose : Frees all actor instances on a given instance list
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/
void FreeAllActorInstances(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawActorInstances(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baDrawActorInstance
*/
void DrawActorInstances(ACTORINSTANCELIST *actorInstanceList, uint32 flags = 0);

/* --------------------------------------------------------------------------------
   Function : DrawActorInstances2(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws all actor instance in the supplied list (CKEY and alpha)
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baDrawActorInstance
*/
void DrawActorInstances2(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawActorInstancesDetails(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws info for all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info :
*/
void DrawActorInstancesDetails(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CalculateCurrentAlpha
   Purpose : Calculate current alpha values
   Parameters : actorInstanceList, min, max, maxSize
   Returns : 
   Info : will fill in currentAlpha and lineThickness part of actor structure using gameStatus.viewPoint
*/
void CalculateCurrentAlpha(ACTORINSTANCELIST *actorInstanceList, float min, float max, float minSize, float maxSize, float sizeMod = 0.0f);

/* --------------------------------------------------------------------------------
   Function : DrawActorInstancesAlpha
   Purpose : Draws all actor instances
   Parameters : actorInstanceList, min, max, maxSize
   Returns : 
   Info : will fade out models smaller that maxSize between the min and max values
*/
void DrawActorInstancesAlpha(ACTORINSTANCELIST *actorInstanceList, float min, float max, float maxSize);

/* --------------------------------------------------------------------------------
   Function : PrepActorInstances(ACTORINSTANCELIST *actorInstanceList
   Purpose : Prepares all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baPrepActorInstance
*/
void PrepActorInstances(ACTORINSTANCELIST *actorInstanceList);

/*  --------------------------------------------------------------------------------
	Function	: AnimateActorInstance
	Purpose		: Update the animation for an actorInstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void AnimateActorInstance(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnimateActorInstances
   Purpose : Updates all the animation information on an actorInstanceList using speedRatio
   Parameters : actorInstanceList
   Returns : 
   Info : calls baUpdateNodes
*/
void AnimateActorInstances(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : EnableActorCartoonRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorCartoonRenderMode(TBActorInstance *actorInstance, TBActRenderMode_CartoonLit *clParms = NULL, TBActRenderMode_Outlined *coParms = NULL);

/* --------------------------------------------------------------------------------
   Function : DisableActorCartoonRenderMode
   Purpose : Disable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void DisableActorCartoonRenderMode(TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SetActorPosition
   Purpose : Sets the current position for an actor
   Parameters : actorInstance, pos
   Returns : 
   Info : 
*/
void SetActorPosition(ACTORINSTANCE *actorInstance,TBVector pos);

/* --------------------------------------------------------------------------------
   Function : SetActorScale
   Purpose : Sets the current position for an actor
   Parameters : actorInstance, pos
   Returns : 
   Info : 
*/
void SetActorScale(ACTORINSTANCE *actorInstance,TBVector scale);

/* --------------------------------------------------------------------------------
   Function : SetActorOrientation
   Purpose : Sets the current orientation for an actor
   Parameters : actorInstance, orientation as quaternion
   Returns : 
   Info : 
*/
void SetActorOrientation(ACTORINSTANCE *actorInstance,TBQuaternion quat);

/* --------------------------------------------------------------------------------
   Function : SetActorOrientation
   Purpose : Sets the current orientation for an actor
   Parameters : actorInstance, orientation as vector
   Returns : 
   Info : 
*/
void SetActorOrientation(ACTORINSTANCE *actorInstance, const TBVector angleVector);

/* --------------------------------------------------------------------------------
   Function : ChangeActorInstance
   Purpose : Change an actor instance to use a different actor, used for costume change
   Parameters : actorInstance, new actor, anim queue size, retarget anim to actor
   Returns : TRUE / FALSE
   Info : By default this will enable cartoon lighting on the new actor
*/
int ChangeActorInstance(ACTORINSTANCE *actorInstance,TBActor *actor, int queueSize, TBActor *retarget, int copy = FALSE);

/* --------------------------------------------------------------------------------
   Function : SetCollisionCallback
   Purpose : Set the function to be called when a collision occurs
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : The function provided will be called when a collision has occured
			the object with the callback is object and the colliding object is collider
*/
void SetCollisionCallback(ACTORINSTANCE *actorInstance,RespondToCollision callback);

/* --------------------------------------------------------------------------------
   Function : SetPreCollisionCallback
   Purpose : Set the function to be called when a collision occurs
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : The function provided will be called when a collision has occured
			the object with the callback is object and the colliding object is collider
*/

void SetPreCollisionCallback(ACTORINSTANCE *actorInstance, PreRespondToCollision callback );

/* --------------------------------------------------------------------------------
   Function : AddInstanceToInstanceList
   Purpose : Add an actorInstance to the instanceList
   Parameters : actorInstance, actorInstanceList
   Returns : 
   Info : 
*/
void AddInstanceToInstanceList(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : RemoveInstanceFromInstanceList
   Purpose : Remove an instance from the instance list
   Parameters : actorInstance, actorInstanceList
   Returns : 
   Info : 
*/
void RemoveInstanceFromInstanceList(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : SetOutsideBoundingBoxCallback
   Purpose : Set the function to be called when leaving bounding box of an object previously stopped collision with
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : 
*/

void SetOutsideBoundingBoxCallback(ACTORINSTANCE *actorInstance, OutsideBoundingBox callback );

/* --------------------------------------------------------------------------------
   Function : DrawActorInstanceZones
   Purpose : draws actor instance zones (yMin, maxRadius, etc..)
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/

void DrawActorInstanceZones( ACTORINSTANCELIST *actorInstanceList );

/* --------------------------------------------------------------------------------
   Function : SubActorFromActorList
   Purpose : Removes and actor from the actor list
   Parameters : identifierName
   Returns : pointer the the TBActor that has been removed from the list or NULL for could'nt find actor
   Info : Remove and actor from the actor list
*/
TBActor *SubActorFromActorList(uint32 crc);

/* --------------------------------------------------------------------------------
   Function : FreeActor
   Purpose : Removes an actor from the actor list and free the memory allocated completely
   Parameters : file name
   Returns : TRUE, FALSE
   Info : 
*/
int FreeActor(uint32 crc);

/*  --------------------------------------------------------------------------------
	Function	: CalculateCurrentAlphaCharacter
	Purpose		: Calculate the current alpha for a character
	Parameters	: actorInstance list
	Returns		: 
	Info		: 
*/

void CalculateCurrentAlphaCharacter(ACTORINSTANCELIST *actorInstanceList);

#endif
