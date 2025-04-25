#ifndef __H_MAPFILE
#define __H_MAPFILE

#include "parsemap.h"
#include "magnet.h"
#include "camera.h"								// PP: camera stuff
#include "colour.h"
#include "tazbossitems.h"
#include "water.h"
#include "zapper.h"
#include "scenicobjects.h"

#define MAX_BREAKABLE_DEBRIS	(7)

typedef struct TAG_INTROINFO
{
	struct		TAG_CAMERACASE	*camCase[10];
	int			numPans;
	int			currentPan;
} INTROINFO, _INTROINFO BALIGN16;

typedef struct TAG_MAPSTATS
{
	int			numPosters;
	int			numDestructibles;
	int			numCollectibles;
	int			numFood;
	int			numBurpCans;
} MAPSTATS, _MAPSTATS BALIGN16;

typedef struct TAG_MAPINFO
{
	char				mapName[32];			// map name
	struct TAG_COLLISIONCACHE	*collisionCache;
	ACTORINSTANCELIST	characterInstances;
	ACTORINSTANCELIST	inactiveCharacterInstances;
	ACTORINSTANCELIST	levelInstances;
	ACTORINSTANCELIST	animatedInstances;
	ACTORINSTANCELIST	fallingInstances;
	ACTORINSTANCELIST	invisibleInstances;
	ACTORINSTANCELIST	collisionInstances;
	ACTORINSTANCELIST	effectsInstances;
	ACTORINSTANCELIST	triggerInstances;
	ACTORINSTANCELIST	statsInstances;
	ACTORINSTANCELIST	phoneBoxInstances;
	int					meshPolys;				// total number of polys in all meshes in map
	int					drawnPolys;				// total number of polys to draw in map
	int					numberOfCollectibles;
	int					numberOfDestructibles;
	int					objectsDrawn;			// TP: number of objects drawn last frame
	TBPackageIndex		*mapPackage;			// pak index for the entire map
	TBSet				*landscape;
	char				background1[64];
	char				background2[64];
	char				background3[64];
	float				backgroundAngle;
	float				backgroundYOffset;
	uint32				startPos;				// start position actor CRC
	uint32				startPos2;				// start position actor CRC
	char				sceneNumber;
	char				lastScene;				// TP: the number of the last scene
	char				chPad;

	INTROINFO			intro;
	MAPSTATS			mapStats;

	class TAG_CUTSCENE	*exitCutScene;			// TP: cutscene to play when level finishes
	class TAG_CUTSCENE	*exitCutScenePt2;			// TP: cutscene to play when level finishes
	class TAG_CUTSCENE	*introCutScene;			// TP: cutscene to play when level begins
	class TAG_CUTSCENE	*introCutScenePt2;		// TP: second cutscene to play when level begins

	class TAG_MAP		*pauseMap;

	struct TAG_TRIGGERINFO	*onLevelComplete;		// TP: message to send when level is completed

	struct TAG_TRIGGERINFO *posterComplete[10];		// TP: messages to send if starting level with posters complete

	bool				fogState;
	float				fogMin,fogMinOrig;
	float				fogMax,fogMaxOrig;
	COLOUR				fogColour;

	TBDebugStream		errorStream;
	TBDebugStream		memoryStream;
	TBDebugStream		optimizeStream;

	int					allPostersDestroyed;		// TP: This flag is set when all the posters on the level have been destroyed
} MAPINFO, _MAPINFO BALIGN16;

typedef struct TAG_MAPOBJECTATTRIBS
{
	char	name[256];
	int		collectible;
	int		collectibleColour[3];
	int		respawn;
	int		destructible;
	int		invisible;
	int		specialType;
	int		cageCRC;

//projectile
	uint32	projectTo;
	float	projectAngle;
	uint32	contact;
	char	cannonEffect;							// PP: index of effect triggered as the "cannon" fires - see CANNONEFFECT_ enum
//tramampolines
	float	springyHeight;
	float	springyRadius;
	int		alwaysBounce;
//steppingstone
	float	steppingStoneHeight;
	float	bobbleTime;
	float	sinkAcceleration;
	float	bobbleHeight;
//tilting
	float	xAngleMax;
	float	zAngleMax;
	float	maxRadius;
	float	maxAcceleration;
	float	retardationForce;
//swinging
	float	length;
	int32	model;
	char	modelName[256];
//falling
	uint32	fallingType;
	uint32	impactType;
	float	fallingRadius;
	int		fallingPeriod;
//water
	uint32	numWaterResetPoints;
	EWaterType	waterType;
//wantedposters
	int32	posterNumber;
	int32	posterSpecialCase;
//mailbox
	int32	mailboxNumber;
//highVoltage
	int32	voltageState;
	int32	initialVoltageState;
//general flags
	char	visible;
	bool	collision;
	char	liveUpdate;
	bool	fog;
//destructibles
	int32	hits;
	int		componentDebris;
	uint32	destroyHit;
	uint32	firstHit;
	uint32	secondHit;
	uint32	idle;
	uint32	sparkTex;
//effects
	uint32	effect;
	float	effectPeriod;
	float	effectXSpeed;
	float	effectYSpeed;
	float	effectZSpeed;
//path flags
	float	radius;
//camera case
	int32	caseType;
	uint32	cameraFollow;
	uint32	cameraLookAt;
	uint32	cameraLookAtNode;
	uint32	cameraFollowNode;
	uint32	cameraDest;
	float	cameraStartDelay;
	float	caseTime;
	float	angle;
	int		smooth;
	int		exitCam;
	int		possnap;
	int		lookatsnap;
	int		possnapback;
	int		lookatsnapback;
	bool	transitionControl;
//snow
	int		snowDensity;
	float	snowDim;
	uint32	snowType;
	float	snowRate;
//electricity
	float	electricityAccuracy;
	char	state;
//fire
	float	fireSize;
//transporter
	uint32	transportTo;
	float	transX;
	float	transY;
	float	transZ;
	float	delay;
	uint32	fadeUp;
	uint32	fadeDown;
//speedboost
	float	speedboost;
	uint32	speedboostTarget;
//randomfood
	int32	foodType;
//vehicles
	int32	vehicleType;
// platforms
	int		platformType;
	int		platformMode;
	uint32	platformCRC;
	bool	platformCameraControl;
	float	returnSpeed;
// doors
	float	doorSpringyness;
	float	doorStiffness;
	int32	doorType;
//laser
	float	laserSize;
	char	laserTexture[256];
//fountain
	float	fountainMaxHeight;
	float	fountainPeriod;
	int		fountainIdle;
//shatter
	uint32	shatterType;
	int		destroy; 
	uint32	triggerType;
	int		money;
	uint32	idleAnimCRC;
	uint32	destroyAnimCRC;
//checkpoints
	int		sequence;
	uchar	r,g,b,a;
	float	width;
//magnets
	float	magnetMin;
	float	magnetMax;
//flocking
	float	safetyRange;
	int		numFlockers;
	uint32	deadFlockerCRC;
	uint32	flockerSprayCRC;

	uchar	footstep;
//	security boxes
	uint32	triggerCRC;
// front end camera
	int32	frontEndCamera;
	int32	flags;
	bool	fade;
// haar
	float	density;
// tazboss2
	ETazBossItem	itemType;
	uint32			replacementCRC;
	uint32			animCRC;
	uint32			camNumber;
//musicbox
	float	musicBoxRadius;
	uint32	musicBoxInstanceCRC;
	int		musicBoxState;
	bool	useBox;
//zapper
	EZapperType		zapperType;
	int		haloColour[3];
//tarbubble
	uint32	initState;
//scenic	
	EScenic	scenicType;
//reverb - GameCube only
#if BPLATFORM==GAMECUBE
	int		reverbType;
#endif
}MAPOBJECTATTRIBS;

/* --------------------------------------------------------------------------------
   Function : FindActorInstanceInMap
   Purpose : Locates an actor instance in map
   Parameters : instance name, or crc, instance list
   Returns : ACTORINSTANCE or NULL
   Info : 
*/
ACTORINSTANCE *FindActorInstanceInMap(char *name, uint32 crc, MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function		: FreeActorInstances
   Purpose		: Free all instances of an actor
   Parameters	: (opt/NULL=all lists) ptr to instance list on which to find instances
   Returns		: 
   Info			: // PP: not too sure where this should go
*/
void FreeActorInstances(const char* const actorName, ACTORINSTANCELIST* const instanceList=NULL);

/* --------------------------------------------------------------------------------
   Function : InitialiseMap
   Purpose : Load in a map file and create the scene
   Parameters : 
   Returns : 
   Info : 
*/
int	InitialiseMap(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : PrepareMap
   Purpose : Prepare a map for drawing
   Parameters : 
   Returns : 
   Info : 
*/
int	PrepareMap(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : DrawMap
   Purpose : Draw a map
   Parameters : 
   Returns : 
   Info : 
*/
int	DrawMap(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : DrawMap2
   Purpose : Draw alpha and ckey components of a map
   Parameters : 
   Returns : 
   Info : 
*/
int	DrawMap2(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : UpdateMap
   Purpose : Update a map
   Parameters : 
   Returns : 
   Info : 
*/
int	UpdateMap(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : FreeMap
   Purpose : free a map
   Parameters : 
   Returns : 
   Info : 
*/
int	FreeMap(MAPINFO *map);

/* --------------------------------------------------------------------------------
   Function : LoadLOMFile
   Purpose : example of how load and use a .lom file
   Parameters : pointer to .lom file name
   Returns : 1 - okay, 0 - error
   Info : 
*/
int LoadLOMFile(char *filename);

/* --------------------------------------------------------------------------------
   Function : collateObjectsNodes
   Purpose : node enumeration callback. collate all "Object" nodes in the level tree
   Parameters : node class pointer, context [int *]
   Returns : NENUM_CONTINUE - continue search, NENUM_END - terminate search, NENUM_ENDPATH - don't search below this node
   Info : 
*/
int collateObjectsNodes(struct TAG_CPMNode *node, void *context);

/* --------------------------------------------------------------------------------
   Function : collateObjectInstances
   Purpose : node enumeration callback. collate all "Instance" nodes in this branch
   Parameters : node class pointer, context [float *]
   Returns : NENUM_CONTINUE - continue search, NENUM_END - terminate search, NENUM_ENDPATH - don't search below this node
   Info : 
*/
int collateObjectInstances(struct TAG_CPMNode *node, void *context);

/* --------------------------------------------------------------------------------
   Function : FillInAttributes
   Purpose : read know attributes from node and fills in attribute structure
   Parameters : pointer to node
   Returns : 
   Info : 
*/
void FillInAttributes(struct TAG_MAPOBJECTATTRIBS *attribs,CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : ChangeAttributes
   Purpose : alter existing attributes based on new node
   Parameters : pointer to node
   Returns : 
   Info : 
*/
void ChangeAttributes(struct TAG_MAPOBJECTATTRIBS *attribs, CPMNode *node);

int ReceiveAttrib(CPMAttrib *attrib,void *context);

struct TAG_TRIGGERINFO *GetTrigger(char *type, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : ReceiveWaterAttribAndStoreObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveWaterAttribAndStoreObject(CPMAttrib *attrib,void *context);

/* --------------------------------------------------------------------------------
   Function : GetWaterResetObjectsFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

struct TAG_WATERRESETINFO *GetWaterResetObjectsFromMap(char *type, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : ReceiveElectricityPathAttribAndStoreObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveElectricityPathAttribAndStoreObject(CPMAttrib *attrib,void *context);

/* --------------------------------------------------------------------------------
   Function : GetElectricityPathNodesFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

struct TAG_ELECTRICITYPATHINFO *GetElectricityPathNodesFromMap(char *type, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : ReceiveSparkAttribAndStoreTexture
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveSparkAttribAndStoreTexture(CPMAttrib *attrib,void *context);

/* --------------------------------------------------------------------------------
   Function : GetSparkTexturesFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

struct TAG_SPARKINFO *GetSparkTexturesFromMap(char *type, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : GetDebrisObjectsFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

struct TAG_DESTRUCTIBLEDEBRISINFO *GetDebrisObjectsFromMap(char *type, CPMNode *node);

/* --------------------------------------------------------------------------------
   Function : ReceiveDebrisAttribAndStore
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveDebrisAttribAndStore(CPMAttrib *attrib,void *context);

void DrawMapSet(MAPINFO *map,struct TAG_CAMERAINFO *cameraInfo);

void DrawMapSetAlpha(MAPINFO *map,struct TAG_CAMERAINFO *cameraInfo);

void UpdateInstanceList(ACTORINSTANCELIST *actorInstanceList);

/*  --------------------------------------------------------------------------------
	Function	: SendPosterCompleteMessages
	Purpose		: Based on the posters complete this send messages to replicate the effect of them being destroyed etc.
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SendPosterCompleteMessages();


/*  --------------------------------------------------------------------------------
	Function	: IsActorInstanceValid
	Purpose		: Can be used to test if this pointer points to a valid actorInstance structure
	Parameters	: actorInstance
	Returns		: true/false
	Info		: 
*/

bool IsActorInstanceValid(ACTORINSTANCE *actorInstance);

#endif