#ifndef __H_PHILSCENE
#define __H_PHILSCENE



// PP: TEST
extern bool	frameBlur;


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePhilGame
	Purpose 	: // PP: Initialise stuff for me at the beginning of the game
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitialisePhilGame(void);


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePhilScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialisePhilScene(void);

/*	--------------------------------------------------------------------------------
	Function 	: PrepPhilScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void PrepPhilScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: DrawPhilScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawPhilScene(int scene,struct TAG_CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePhilScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdatePhilScene(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: FreePhilScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreePhilScene(int scene);


#endif// PP: def __PHILSCENE_H_