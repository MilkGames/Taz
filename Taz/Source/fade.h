#ifndef __H_FADE
#define __H_FADE

enum EFadeTypes{
	FADETYPE_NORMAL,
	FADETYPE_CIRCLE,
	FADETYPE_CIRCLEWB,
	FADETYPE_SWIPE,
};

enum EFadeStates{
	FADESTATE_OFF,				// TP: there is no fade present on screen
	FADESTATE_DOWN,				// TP: start fading to black
	FADESTATE_ON,				// TP: the fade has completely covered the screen
	FADESTATE_UP,				// TP: start fading to transparent
};

#define FADESCENE_NONE (-1)
#define FADESCENE_DEATH (-2)
#define FADESCENE_CAGED (-3)

typedef void (*FadeFinishedCallback)(class TAG_FADE *fade);

typedef class TAG_FADE
{
	TBPrimVertex	screenPolyVerts[4];
	TBPrimVertex	circlePolyVerts[91*2];
	TBPrimVertex	circleEdgePolyVerts[91*2];
	TBPrimVertex	swipePolyVerts[6];
	TBPrimVertex	tintPolyVerts[4];

	TBVector		innerCircle[91];
	TBVector		circleFadeTo;				// stores the point the circle should fade onto
	TBVector		swipeVerts[6];

	EFadeStates		status;
	EFadeTypes		fadeType;
	float			swipeAngleDown;				// angle of swipe in rads 0 vertical for fade down
	float			swipeAngleUp;				// angle of swipe in rads 0 vertical for fade up (direction reversed)

	float			distanceToScreenCorner;	// distance from the center of the screen to the corner
	float			fadeValue;				// 0-255 current fade value
	float			fadeSpeed;				// time to complete the fade in seconds
	int32			sceneToLoad;			// FADESCENE_NONE if no load

	struct TAG_GAMESTATUS		*gameStatusFade;
	FadeFinishedCallback		fadeUpFinishedCallback;	//called when fade is finished
	FadeFinishedCallback		fadeDownFinishedCallback;
	bool						fadeUpOnCompletion;		// start fade up automaticaly on finishing fade down
	bool						levelCompleteOnFadeDown;// TP: Calls the level complete function when the fade down is complete
	uchar						r,g,b,a;				// TP: Constant screen tint to apply to camera, a = 0 turns effect off

	ACTORINSTANCE	*pPlayer;							// CPW: temp stinky nasty bodge

	bool			access;								// TP: is access to this fade currently available
	bool			secureFade;							// TP: set to true to ensure the fade cannot be interrupted

	/*  --------------------------------------------------------------------------------
		Function	: Access
		Purpose		: Are changes to the fade info currently allowed
		Parameters	: 
		Returns		: true/false
		Info		: 
	*/
	bool	Access();

	/*	--------------------------------------------------------------------------------
		Function 	: SetFadeState
		Purpose 	: update the fade structure with the current state
		Parameters 	: FADE
		Returns 	: true/false
		Info 		: 
	*/
	bool SetFadeState();

public:

	uint32			xDoorCRC;							// TP: crc of door to use when level starts or 0

	/*	--------------------------------------------------------------------------------
		Function 	: InitFade
		Purpose 	: Initialise the fade structure
		Parameters 	: FADE
		Returns 	: 
		Info 		:
	*/
	void InitFade();

	/*	--------------------------------------------------------------------------------
		Function 	: DrawFade
		Purpose 	: Draw the fade in its current state
		Parameters 	: FADE
		Returns 	: 
		Info 		:
	*/
	void DrawFade();

	/*  --------------------------------------------------------------------------------
		Function	: SetPlayer
		Purpose		: Set the player that this fade is associated with
		Parameters	: actorInstance of player
		Returns		: 
		Info		: 
	*/

	bool SetPlayer(ACTORINSTANCE *player);

	/*  --------------------------------------------------------------------------------
		Function	: SetSceneToLoad
		Purpose		: Set the scene to be loaded when the fade down is complete
		Parameters	: scene number
		Returns		: true/false
		Info		: 
	*/
	bool SetSceneToLoad(int scene);

	/*  --------------------------------------------------------------------------------
		Function	: SetGameStatus
		Purpose		: Set the current game status structure the camera should refer to 
		Parameters	: gameStatus
		Returns		: true/false
		Info		: 
	*/
	bool SetGameStatus(TAG_GAMESTATUS *gameStatus);

	/*  --------------------------------------------------------------------------------
		Function	: LevelCompleteOnFadeDown
		Purpose		: Time a level complete call with the completion of the fade down
		Parameters	: state = true/false
		Returns		: true/false
		Info		: 
	*/
	bool LevelCompleteOnFadeDown(bool state);

	/*  --------------------------------------------------------------------------------
		Function	: FadeUpOnCompletion
		Purpose		: Fade up after fade down is complete
		Parameters	: true/false
		Returns		: true/false
		Info		: 
	*/
	bool FadeUpOnCompletion(bool state);
	
	/*  --------------------------------------------------------------------------------
		Function	: GetPlayer
		Purpose		: Return the player that this fade is associated with
		Parameters	: 
		Returns		: ACTORINSTANCE of player
		Info		: 
	*/

	ACTORINSTANCE *GetPlayer();

	/*	--------------------------------------------------------------------------------
		Function 	: UpdateFade
		Purpose 	: Updates the fade
		Parameters 	: FADE
		Returns 	: 
		Info 		: uses speedRatio to determine how much to advance the fade
	*/
	void UpdateFade();

	/*  --------------------------------------------------------------------------------
		Function	: SetFadeStatus
		Purpose		: Set the current status of the fade
		Parameters	: state
		Returns		: true/false
		Info		: 
	*/
	bool SetFadeStatus(EFadeStates state);

	/*	--------------------------------------------------------------------------------
		Function 	: StartFadeDown
		Purpose 	: Start a fade down of the specified type
		Parameters 	: FADE, type, speed in seconds to make complete fade
		Returns 	: true/false
		Info 		: 
	*/
	bool StartFadeDown(EFadeTypes type, float speed, bool secure=false);

	/*	--------------------------------------------------------------------------------
		Function 	: StartFadeUp
		Purpose 	: Start a fade up of the specified type
		Parameters 	: FADE, type, speed in seconds
		Returns 	: true/false
		Info 		: 
	*/
	bool StartFadeUp(EFadeTypes type, float speed, bool secure=false);

	/*	--------------------------------------------------------------------------------
		Function 	: FadeAndChangeScene
		Purpose 	: Fades down then frees the current scene and load the new one, then fade up
		Parameters 	: FADE, type, scene number,speed
		Returns 	: true/false
		Info 		: calls FreeScene and InitialiseScene
	*/
	bool FadeAndChangeScene(struct TAG_GAMESTATUS *gameStatus, EFadeTypes type, int scene, float speed, uint32 xDoorCRC = 0, bool fadeUpOnCompletion=false);

	/*	--------------------------------------------------------------------------------
		Function 	: FinishedFade
		Purpose 	: has the fade finished
		Parameters 	: FADE
		Returns 	: TRUE / FALSE
		Info 		: 
	*/
	int FinishedFade();

	/*	--------------------------------------------------------------------------------
		Function 	: SetSwipeDirection
		Purpose 	: set the current direction for the swipe.
		Parameters 	: FADE, angle in rads 0 - vertical for fade down, angle for fade up
		Returns 	: nothing
		Info 		: 
	*/
	bool SetSwipeDirection(float fadeDownAngle,float fadeUpAngle);

	/*	--------------------------------------------------------------------------------
		Function 	: SetCircleFadeTo
		Purpose 	: sets the screen coordinate to fade the circle onto
		Parameters 	: FADE, x and y coords
		Returns 	: nothing
		Info 		: 
	*/
	bool SetCircleFadeTo(float x,float y);

	/*	--------------------------------------------------------------------------------
		Function 	: GetFadeState
		Purpose 	: returns the current state of the fade
		Parameters 	: FADE
		Returns 	: 
		Info 		: 
	*/
	int GetFadeState();

	/*	--------------------------------------------------------------------------------
		Function 	: SetFadeFinishedCallback
		Purpose 	: sets the function to be called when a fade is finished
		Parameters 	: FADE
		Returns 	: true/false
		Info 		: 
	*/
	bool SetFadeUpFinishedCallback(FadeFinishedCallback callback);

	/*	--------------------------------------------------------------------------------
		Function 	: SetFadeFinishedCallback
		Purpose 	: sets the function to be called when a fade is finished
		Parameters 	: FADE
		Returns 	: true/false
		Info 		: 
	*/
	bool SetFadeDownFinishedCallback(FadeFinishedCallback callback);

	/*	--------------------------------------------------------------------------------
		Function 	: SetFadeTint
		Purpose 	: Sets the current colour and alpha for the camera
		Parameters 	: FADE, r, g, b, a
		Returns 	: 
		Info 		: 
	*/
	bool SetFadeTint(uchar r, uchar g, uchar b, uchar a);

	/*  --------------------------------------------------------------------------------
		Function	: SetFadeUp
		Purpose		: Sets the fade in the off state immediately so there is no fade present
		Parameters	: fade
		Returns		: true/false
		Info		: 
	*/

	bool SetFadeUp();

	/*  --------------------------------------------------------------------------------
		Function	: SetFadeDown
		Purpose		: Sets the fade in the on state immediately so the screen will go black
		Parameters	: fade
		Returns		: true/false
		Info		: 
	*/

	bool SetFadeDown();

	/*  --------------------------------------------------------------------------------
		Function	: FADE::OkToChangeFade
		Purpose		: Check if it is ok to change the current fade settings
		Parameters	: fade structure
		Returns		: true/false
		Info		: 
	*/

	bool OkToChangeFade();


	/*  --------------------------------------------------------------------------------
		Function	: GetFadeValue
		Purpose		: Return the current fade value
		Parameters	: 
		Returns		: fadeValue
		Info		: 
	*/

	float GetFadeValue();

	/*  --------------------------------------------------------------------------------
		Function	: GetFadeSpeed
		Purpose		: Return the current fade speed
		Parameters	: 
		Returns		: fadeSpeed
		Info		: 
	*/

	float GetFadeSpeed();

	/*  --------------------------------------------------------------------------------
		Function	: GetSceneToLoad
		Purpose		: Return the scene to be loaded
		Parameters	: 
		Returns		: scene number
		Info		: 
	*/

	int GetSceneToLoad();

	/*  --------------------------------------------------------------------------------
		Function	: GetFadeType
		Purpose		: return the current fade type
		Parameters	: 
		Returns		: EFadeTypes
		Info		: 
	*/

	EFadeTypes GetFadeType();

	/*  --------------------------------------------------------------------------------
		Function	: FADE::Lock
		Purpose		: Must be called before using any of the fade functions
		Parameters	: 
		Returns		: true if fade is locked and available / false if fade is not available for use
		Info		: 
	*/

	bool Lock();


	/*  --------------------------------------------------------------------------------
		Function	: FADE::Unlock
		Purpose		: Called to unlock a fade, fade functions 
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	void Unlock();

	/*  --------------------------------------------------------------------------------
		Function	: FADE::SecureFade
		Purpose		: Tells the fade handler that no other fade instructions should be processed during this fade
					:	NOTE: this is set automatically if the fadeandchangescene function is used to change level or cage taz
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	void SecureFade();

	/*  --------------------------------------------------------------------------------
		Function	: FADE::RemoveSecureFade
		Purpose		: Takes control of the fade regardless of the secure setting
		Parameters	: 
		Returns		: 
		Info		: ***WARNING*** This function should only be used by the demo modes to exit on a start press
	*/

	bool RemoveSecureFade();

} FADE;



#endif