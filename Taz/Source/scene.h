#ifndef __H_SCENE
#define __H_SCENE

#include "collisioncache.h"
#include "camera.h"
#include "mapfile.h"
//#include "text.h"

#define IN_HUB	((map.sceneNumber==SCENE_ZOOHUB)||(map.sceneNumber==SCENE_CITYHUB)||(map.sceneNumber==SCENE_WILDWESTHUB))
#define IN_ZOO  ((map.sceneNumber>=SCENE_ZOOHUB)&&(map.sceneNumber<=SCENE_ZOOBOSS))
#define IN_CITY  ((map.sceneNumber>=SCENE_CITYHUB)&&(map.sceneNumber<=SCENE_CITYBOSS))
#define IN_WEST  ((map.sceneNumber>=SCENE_WILDWESTHUB)&&(map.sceneNumber<=SCENE_WESTBOSS))
#define IN_TAZ  ((map.sceneNumber>=SCENE_TAZHUB)&&(map.sceneNumber<=SCENE_TAZBOSS))
#define IN_VR	((map.sceneNumber>=SCENE_MUSEUMVEHICLERACE)&&(map.sceneNumber<=SCENE_DEPTSTRVEHICLERACE))
#define IN_RACE ((map.sceneNumber>=SCENE_ICEDOMERACE)&&(map.sceneNumber<=SCENE_SAFARIRACE))
#define IN_DEST ((map.sceneNumber>=SCENE_DESTRUCTIONAQUA)&&(map.sceneNumber<=SCENE_DESTRUCTIONTASMANIA))

enum EScene{
	SCENE_LANGUAGESELECT,
	SCENE_FRONTEND,
	SCENE_PLAYPEN,	

	SCENE_ZOOHUB,
	SCENE_ICEDOME,
	SCENE_SAFARI,
	SCENE_AQUA,
	SCENE_ZOOBOSS,

	SCENE_CITYHUB,
	SCENE_DEPTSTR,
	SCENE_MUSEUM,
	SCENE_CONSTRUCT,
	SCENE_CITYBOSS,

	SCENE_WILDWESTHUB,
	SCENE_GHOSTTOWN,
	SCENE_GOLDMINE,
	SCENE_GRANDCANYON,
	SCENE_WESTBOSS,

	SCENE_TAZHUB,
	SCENE_TAZPREBOSS,
	SCENE_TAZBOSS,

	SCENE_MUSEUMVEHICLERACE,
	SCENE_GRANDCVEHICLERACE,
	SCENE_DEPTSTRVEHICLERACE,

	SCENE_DESTRUCTIONAQUA,
	SCENE_DESTRUCTIONCONSTRUCT,
	SCENE_DESTRUCTIONTASMANIA,

	SCENE_ICEDOMERACE,
	SCENE_GOLDMINERACE,
	SCENE_SAFARIRACE,

	// any scenes after TESTCUTSCENE can only be accessed if l2 r2 and triangle are held down
	SCENE_MAP,
	SCENE_TESTMAP,
	SCENE_NOSCENE,
	SCENE_OBJECTVIEWONLY,
	SCENE_TAZINTROCUTSCENE,
	SCENE_STARTGAMEINTRO,

	SCENE_DEMODEPT,	// JW: Stripped down version of department store for Demo

	ENDOFSCENES,
	// any scenes after ENDOFSCENES are disabled
};

#define SCENE_NUMSCENES	(SCENE_MAP)

struct DemoModeData{
	int	numBytes;
	int	numSessBytes;
	int	numFpsBytes;

	float				*fps;
	TBInputRecording	*session;
};

extern struct TAG_BOOK		*demoBook;

#ifndef STANDALONE
#ifdef CONSUMERDEMO
	extern struct TAG_BOOK	*demoTimerBook;

	extern float	demoGameTimer;
#endif
#endif

extern	TBInputRecording	*sessionRecPtr;
extern	float				*fpsStore;
extern	DemoModeData		*dmData;	

extern char *sceneText[];
extern char *sceneFileName[];


enum ESceneType{
	SCENETYPE_INGAME,
	SCENETYPE_CUTSCENE,
	SCENETYPE_INTRO,
};

extern ACTORINSTANCELIST		characterInstances;
extern ACTORINSTANCELIST		levelInstances;
extern ACTORINSTANCELIST		animatedInstances;
extern ACTORINSTANCELIST		rockInstances;
extern ACTORINSTANCELIST		tempInstances;
extern ACTORINSTANCELIST		cameraInstances;
extern ACTORINSTANCELIST		invisibleInstances;
extern ACTORINSTANCELIST		statsInstances;


extern ACTORINSTANCE			*tazInstance;
extern ACTORINSTANCE			*levelInstance;
extern COLLISIONCACHE			collisionCache;
extern struct TAG_ENVMAP		environmentMap;
extern struct TAG_MAPINFO		map;


extern TBLightsource *dirLight1,*dirLight2,*dirLight3,*ambient;


#ifdef	DEBUGGING
extern ACTORINSTANCE			*tazSphereInstance;
#endif

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: 
	Info 		:
*/
int InitialiseScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: PrepareScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int PrepareScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: StartSceneLevelDraw
	Purpose 	: Draws the background and then initiates level draw
	Parameters 	: scene number, camera to use
	Returns 	: 
	Info 		: This only starts the level drawing, it does not wait for it to finish
*/
void StartSceneLevelDraw(int scene,CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: StartSceneLevelDrawAlpha
	Purpose 	: Draws the background and then initiates level draw
	Parameters 	: scene number, camera to use
	Returns 	: 
	Info 		: This only starts the level drawing, it does not wait for it to finish
*/
void StartSceneLevelDrawAlpha(int scene,CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: DrawScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int DrawScene(int scene,CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: DrawScene2
	Purpose 	: Draws all alpha polygons in scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int DrawScene2(int scene,CAMERAINFO *cameraInfo, int options);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateScenePreLevel
	Purpose 	: Update scene prior to beginning drawing level
	Parameters 	: scene number
	Returns 	: 
	Info 		: This is a safe place to request level change etc.
*/
void UpdateScenePreLevel(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int UpdateScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: FreeScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int FreeScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: LoadGenericObjects
	Purpose 	: Load all generic objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadGenericObjects(int sceneNumber);

/* --------------------------------------------------------------------------------
   Function : CreateSceneMenu
   Purpose : create and setup the scene menu bar
   Parameters : 
   Returns : 
   Info : 
*/
void CreateSceneMenu(void);

/*	--------------------------------------------------------------------------------
	Function 	: LoadLevelSpecificObjects
	Purpose 	: Load all level specific objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadLevelSpecificObjects(int sceneNumber);

/*	--------------------------------------------------------------------------------
	Function 	: FreeLevelSpecificObjects
	Purpose 	: Free all level specific objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeLevelSpecificObjects(int sceneNumber);

/*	--------------------------------------------------------------------------------
	Function 	: LoadPermanentResources
	Purpose 	: Load all generic objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadPermanentResources(void);

/*	--------------------------------------------------------------------------------
	Function 	: FreePermanentResources
	Purpose 	: Free all permanent resources
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreePermanentResources(void);

/*  --------------------------------------------------------------------------------
	Function	: DrawMemLeakWarning
	Purpose		: If there are memory leaks in the level this will display a warning
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DrawMemLeakWarning(void);

/*	--------------------------------------------------------------------------------
	Function 	: CompleteLoad
	Purpose 	: The items are loaded so read contents
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CompleteLoad(int sceneNumber);

/*	--------------------------------------------------------------------------------
	Function 	: StartLoading
	Purpose 	: Start the background load of needed items
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StartLoading(int sceneNumber);

/*  --------------------------------------------------------------------------------
	Function	: ApplyDecalZMask
	Purpose		: apply the specified actor instance as a z-mask for subsequent decal effects (eg. shadows)
	Parameters	: actor instance to use as a z-mask, (opt/none) maximum Y offset of decals to mask
	Returns		: 
	Info		: // PP: This draws the actor insatnce, with some z bias towards the viewer, to the z-buffer only.
					It makes its own sensible judgement about how far to zbias, based on the platform.
*/
void ApplyDecalZMask(TBActorInstance* const actorInstance, const float yOffset=0.0f);


#endif