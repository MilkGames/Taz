#ifndef __H_CHRISSCENE
#define __H_CHRISSCENE

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseChrisScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseChrisScene(int scene, struct TAG_CAMERAINFO *cameraInfo, struct TAG_CAMERAINFO *cameraInfo2);

/*	--------------------------------------------------------------------------------
	Function 	: PrepChrisScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void PrepChrisScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: DrawChrisScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawChrisScene(int scene,struct TAG_CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateChrisScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateChrisScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: FreeChrisScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeChrisScene(int scene);

#endif
