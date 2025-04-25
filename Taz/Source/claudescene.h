#ifndef __H_CLAUDESCENE
#define __H_CLAUDESCENE

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseClaudeScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseClaudeScene(int scene, CAMERAINFO *cameraInfo, CAMERAINFO *cameraInfo2);

/*	--------------------------------------------------------------------------------
	Function 	: PrepClaudeScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void PrepClaudeScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: DrawClaudeScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawClaudeScene(int scene,CAMERAINFO *cameraInfo);


/*	--------------------------------------------------------------------------------
	Function 	: UpdateClaudeScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateClaudeScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: FreeClaudeScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeClaudeScene(int scene);

#endif
