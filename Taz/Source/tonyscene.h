#ifndef __H_TONYSCENE
#define __H_TONYSCENE

#ifdef TONY
	#undef START_SCENE
	#define START_SCENE			SCENE_LANGUAGESELECT// PP: SCENE_GHOSTTOWN
#endif

// PP: define PHILSCENE to call Phil-specific stuff from TonyScene
// PP: #define PHILSCENE// PP: TEMP! REMEMBER TO REMOVE THIS RIGHT?

#ifdef PHILSCENE
	// PP: not undef'ing cos I want the warning
	#define START_SCENE			SCENE_MUSEUM
#endif// PP: def PHILSCENE

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseTonyGame
	Purpose 	: Initialise Tony stuff at start of game
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitialiseTonyGame(void);


/*	--------------------------------------------------------------------------------
	Function 	: InitialiseTonyScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseTonyScene(void);

/*	--------------------------------------------------------------------------------
	Function 	: PrepTonyScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void PrepTonyScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: DrawTonyScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawTonyScene(int scene,struct TAG_CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTonyScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateTonyScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: FreeTonyScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeTonyScene(int scene);

#endif