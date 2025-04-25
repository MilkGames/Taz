#ifndef __H_PLAYER
#define __H_PLAYER


// PP: TEMP?: radius of Taz for shadow map
// PP: TODO (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_TAZ			METERTOUNIT(3.0f)	// PP: this should be kept as small as possible!!


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePlayers
	Purpose 	: Initialse the player array at beginning of game
	Parameters 	: 
	Returns 	: 
	Info 		: Should be called once at beginning of game
*/
void InitialisePlayers(void);

/*	--------------------------------------------------------------------------------
	Function 	: SetupPlayer
	Purpose 	: Setup a player
	Parameters 	: playerNumber, actorInstance, cameraInfo, controller
	Returns 	: PLAYER
	Info 		: Called to setup a player
*/
struct TAG_PLAYER *SetupPlayer(int playerNumber, struct TAG_ACTORINSTANCE *actorInstance, struct TAG_CAMERAINFO *cameraInfo, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePlayers
	Purpose 	: Create the players and cameras needed for the current scene
	Parameters 	: scene
	Returns 	: 
	Info 		: 
*/
void CreatePlayers(int scene);


ACTORINSTANCE *LoadGameCharacter(char *name, char *id);

#define LoadDaffy		LoadGameCharacter("daft.obe", "daffy")
#define LoadSylvester	LoadGameCharacter("sylvester.obe", "sylvester")
#define LoadElmer		LoadGameCharacter("mrfudd.obe", "elmer")
#define LoadTweety		LoadGameCharacter("tweety.obe", "tweety")


#endif