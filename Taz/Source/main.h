#ifndef __H_MAIN
#define __H_MAIN

#include "actors.h"
#include "splashscreen.h"
#include "Dungeon.h"						// PP: Pieces of code so ugly that they have to be kept out of public view
#include "GlobalSettings.h"					// PP: Global settings adjustable from the front end, eg. volume levels

enum
{
	ML_UPDATEPRELEVEL	= 1<<0,
	ML_UPDATEEXTRAS		= 1<<1,
	ML_DRAWSCENE		= 1<<2,
	ML_UPDATESCENE		= 1<<3,
	ML_PREPARESCENE		= 1<<4,
	ML_PROCESSINPUT		= 1<<5,
	ML_UPDATECAMERA		= 1<<6,
	ML_DRAWEXTRAS		= 1<<7,
	ML_UPDATEFADE		= 1<<8,
	ML_PAPERBOOK		= 1<<9,				// PP: draw and update the paper book currently being displayed
	ML_CUTSCENE			= 1<<10,			// TP: flag to alter the draw order to avoid cutscene syncing problems

	ML_ALL				= (0xffffffff)&(~ML_CUTSCENE),
};

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TEXT ROUTINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: safe strncpy: copies _num-1 characters from _src to _dst, then null-terminates _dst
// PP: _num must be no greater than the total number of characters in _dst
#define STRNCPY(_dst, _src, _num)					{strncpy(_dst, _src, (_num)-1); (_dst)[(_num)-1]='\0';}

/*	--------------------------------------------------------------------------------
	Function 	: strcatf
	Purpose 	: sprintf onto the end of a string
	Parameters 	: destination buffer, format string, argument list
	Returns 	: completed string
	Info 		: 
*/
char* strcatf(char * const buffer, const char * const format, ...);

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MEMORY MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: macro to delete non-null objects
#define SAFE_DELETE(X)								{ if (X) delete (X); (X) = NULL; }

// PP: macro to delete non-null arrays
#define SAFE_DELETE_ARRAY(X)						{ if (X) delete[] (X); (X) = NULL; }

// PP:  macro to free non-null objects
#define SAFE_FREE(X)								{ if (X) FREE(X); (X) = NULL; }

// PP:  macro to zero a structure
#define ZERO(X)										memset(&(X), 0, sizeof(X))

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GRAPHICS MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: macro for setting the position of a prim vertex using a TBVector
#define BDVERTEX_SETPOS(_vertex, _posVector)		BDVERTEX_SETXYZW(_vertex, (_posVector)[X], (_posVector)[Y], (_posVector)[Z], (_posVector)[W])

// PP: macro for getting the position of a prim vertex as a TBVector
#define BDVERTEX_GETPOS(_vertex, _posVector)		BDVERTEX_GETXYZW(_vertex, (_posVector)[X], (_posVector)[Y], (_posVector)[Z], (_posVector)[W])

// PP: macro for setting the colour of a prim vertex using a COLOUR struct
#define BDVERTEX_SETCOLOUR(_vertex, _col)			BDVERTEX_SETRGBA(_vertex, (_col).r, (_col).g, (_col).b, (_col).a)

// PP: macro for getting the colour of a prim vertex as a COLOUR struct
#define BDVERTEX_GETCOLOUR(_vertex, _col)			BDVERTEX_GETRGBA(_vertex, (_col).r, (_col).g, (_col).b, (_col).a)

// PP: macro for setting the position of a prim vertex using a VEC
#define BDVERTEX_SETVEC(_vertex, _vec)				BDVERTEX_SETPOS(_vertex, _vec)// PP: BDVERTEX_SETPOS uses VEC::operator []

// PP: macro for getting the position of a prim vertex as a VEC
#define BDVERTEX_GETVEC(_vertex, _vec)				BDVERTEX_GETPOS(_vertex, _vec)// PP: BDVERTEX_SETPOS uses VEC::operator []

#define MAX_ALPHA_TEX								128		// PP: maximum alpha for textured polys
#define MAX_ALPHA_UNTEX								255		// PP: maximum alpha for untextured polys

// PP: bdSetZBias needs WAAY different values between PC & PS2 - I'm hoping this will help, but I've got a nasty feeling it's
// PP: not as simple as this.
#if BPLATFORM == PS2

#define SETZBIAS(_zBias)							bdSetZBias((_zBias)*50000)// PP: was 100000// PP: was 300000; seems Babel has changed.  Like I said, I think this macro is a sham anyway - but the intentions were good

#else

#define SETZBIAS(_zBias)							bdSetZBias(_zBias)

#if BPLATFORM != PC
//#error //PP: (I tried using #warning but it didn't work)  *** I'm assuming PC Z-bias values will work on this platform(?) ***
#endif

#endif

// PP: Z bias to set when you want something right at the very back of the Z buffer
#if(BPLATFORM == PS2)
#define MAX_ZBIAS									1000000.0f		// PP: ?
#else// PP: if(BPLATFORM == PS2) else
#define MAX_ZBIAS									0.999f
#endif// PP: if(BPLATFORM == PS2) else

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TIMING MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: start timing sumfin
#define STARTTIMER			{ \
								TBTimerValue		_startTime; \
								\
								_startTime=bkTimerRead();

// PP: stop timin sumfin (_name describes wot it was you woz timin)
#define ENDTIMER(_name)			TBTimerValue		_endTime; \
								TBTimerValue		_deltaTime; \
								\
								_endTime=bkTimerRead(); \
								\
								_deltaTime=bkTimerDelta(_startTime,_endTime); \
								\
								bkPrintf("*** TIMER: \"%s\" took %5.2f scanlines ***\n", _name, bkTimerToScanlines(_deltaTime, 60, NULL)); \
							}

// PP: Pretty self-explanatory, so I've spared you the futile descriptive comments...

#define SETPROJECTION(_mode)		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE##_mode);

// PP: pretty rare that we'd want to use this
#define ENABLEBLEND(_enable)		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, _enable, FALSE);

#define SETBLEND(_mode)				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_##_mode, 0); \
									bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_##_mode, 1);

// PP: I don't like having to specify a texture stage when we never use anything other than 0
#define SETTEXTURE(_texture)		bdSetTexture(0, _texture);

// PP: GOD - DAMN I'M LAZY!
#define NOTEXTURE					SETTEXTURE(NULL);

#define SAVESTATES					TBSavedRenderStates		_savedRenderStates; \
									bdSaveRenderStates(&_savedRenderStates);

#define RESTORESTATES				bdRestoreRenderStates(&_savedRenderStates);

#define SETZTEST(_zTest)			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_##_zTest, 0); \
									bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_##_zTest, 1);

#define SETZWRITE(_zWrite)			bdSetRenderState(BDRENDERSTATE_ZWRITE, _zWrite, 0); \
									bdSetRenderState(BDRENDERSTATE_ZWRITE, _zWrite, 1);

// PP: NOTE: IF YOU WANT TO DISABLE FRAMEWRITE, PLEASE USE THE 'DISABLE_FRAMEWRITE' MACRO.
// PP: ONLY USE THE 'SETFRAMEWRITE' MACRO ONCE YOU HAVE ASCERTAINED THAT 'COLOUR_CHANNEL_MASKING_SUPPORTED' IS TRUE
// PP: NOTE: Don't let anyone tell you that the second parameter is ignored!  On PS2, it's not!
#define SETFRAMEWRITE(_frameWrite)	bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, _frameWrite, 0); \
									bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, _frameWrite, 1);

// PP: USE 'DISABLE_FRAMEWRITE' TO DISABLE FRAME WRITING ALTOGETHER
#if(BPLATFORM == PC)// PP: On PC, we can't assume any framewrite control, so we use ((0*srcCol)+(1*destCol)) blending instead
#define DISABLE_FRAMEWRITE			bdPushRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE); \
									bdLockRenderState(BDRENDERSTATE_ALPHAENABLE); \
									bdPushRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND); \
									D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO); \
									D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); \
									bdLockRenderState(BDRENDERSTATE_ALPHABLENDMODE);
#else
// PP: NOTE: You *CANNOT* trust bdPushRenderState and bdPopRenderState, not on PS2 at least.
#define DISABLE_FRAMEWRITE			SETFRAMEWRITE(0)
#endif

// PP: USE 'REENABLE_FRAMEWRITE' TO RE-ENABLE FRAME WRITING TO WHAT IT WAS BEFORE 'DISABLE_FRAMEWRITE' WAS EXECUTED
#if(BPLATFORM == PC)// PP: On PC, we can't assume any framewrite control, so we use ((0*srcCol)+(1*destCol)) blending instead
#define REENABLE_FRAMEWRITE			bdUnlockRenderState(BDRENDERSTATE_ALPHABLENDMODE); \
									bdPopRenderState(BDRENDERSTATE_ALPHABLENDMODE); \
									bdUnlockRenderState(BDRENDERSTATE_ALPHAENABLE); \
									bdPopRenderState(BDRENDERSTATE_ALPHAENABLE);
#else
// PP: NOTE: You *CANNOT* trust bdPushRenderState and bdPopRenderState, not on PS2 at least.
#define REENABLE_FRAMEWRITE			SETFRAMEWRITE(rgbFramewriteChannels)
#endif

// PP: texture wrapping/clamping
#define SETUVWRAP(_uWrap, _vWrap)	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_##_uWrap##U|BDTEXTUREWRAP_##_vWrap##V, FALSE);
#define SETWRAP(_wrap)				SETUVWRAP(_wrap, _wrap)
#define CLAMPTEXTURE				SETWRAP(CLAMP)
#define WRAPTEXTURE					SETWRAP(WRAP)

#define SETFOG(_fog)				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, _fog, FALSE);

// PP: put fog back on/back off according to current settings
#define RESTOREFOG					SETFOG(map.fogState)

#define SETFILTERING(_filter)		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_##_filter, 0); \
									bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_##_filter, 1);

		// PP: return filtering to whatever it is normally
#define RESTOREFILTERING			bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, 0); \
									bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, 1);

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GENERAL MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: macro to set a vector's x,y&z values to be those specified by a trio of position defines,
// PP: eg. BLEHPOS_X, BLEHPOS_Y & BLEHPOS_Z
// PP: In this example, you'd just say "SETVECTORPOS(blehPos, BLEHPOS)"
#define SETVECTORPOS(_vec, _pos)	SETVECTOR(_vec, _pos##_X, _pos##_Y, _pos##_Z, 1.0f)

// PP: 'No Default' macro *** MICROSOFT-SPECIFIC ***
// PP: Apparently, this hints to the optimiser that there's no need to worry about switch cases other than the ones you've handled.
// PP: Which sounds good to me!
#ifdef _TAZDEBUG 
#define NODEFAULT   default: ASSERTM(0, "NODEFAULT failed: not all of the possible values are handled by this'ere switch.\n")
#else// PP: release
#if((BPLATFORM == XBOX) || (BPLATFORM == PC))
#define NODEFAULT   default: __assume(0) \
					/* This tells the optimizer that the default */ \
					/* cannot be reached. As so it does not have to generate */ \
					/* the extra code to check that 'p' has a value */ \
					/* not represented by a case arm.  This makes the switch */ \
					/* run faster. */
#else// PP: not Xbox or PC
#define NODEFAULT// PP: does nothing
#endif// PP: not Xbox or PC
#endif// PP: release

// PP: swap two variables '_a' and '_b' of type '_type'
#define SWAP(_a, _b, _type) \
			{ \
				_type	_swp; \
				_swp=_a; \
				_a=_b; \
				_b=_swp; \
			}

// PP: 45°, PI/4
#define QUARTPI										0.78539816339744830961566084581988f

// PP: set a particular flag bit either high or low in a combination of flag bits
// PP: eg. SETFLAG(myFlags, FLAG_SELECTED, true)
#define SETFLAG(_flags, _flag, _state) \
		if(_state) \
		{ \
			(_flags) |= (_flag); \
		} \
		else \
		{ \
			(_flags) &= ~(_flag); \
		}

// PP: find out if a particular flag bit is high in a combination of flag bits
// PP: eg. if(FLAGSET(myFlags, FLAG_SELECTED))
#define FLAGSET(_flags, _flag)						(((_flags) & (_flag)) != 0)

// PP: macro to get the current difficulty level
#define CURRENT_DIFFICULTY							(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty)

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum EGameState{
	GAMESTATE_INITIALISE,
	GAMESTATE_INGAME,
	GAMESTATE_CUTSCENE,
	GAMESTATE_PAUSE,
	GAMESTATE_CUSTOMPAUSE,				// PP: paused but with a custom update function of your choosing.  This replaces GAMESTATE_ENDOFLEVEL.  Another example of it is in the Zoo boss game, where the elephant freezes in its fall for the identification gag
	GAMESTATE_FADING,
	GAMESTATE_SPLASH,
	GAMESTATE_OBJECTVIEWER,
	GAMESTATE_INTRO,
	GAMESTATE_OUTRO,
	GAMESTATE_MEMCARD,
	GAMESTATE_CAMERACASE,
	GAMESTATE_DEMOMENU,
	GAMESTATE_DEMOQUIT,
	GAMESTATE_PAPERBOOK,				// PP: reading a paper book
	GAMESTATE_CREDITS,
	GAMESTATE_BOSSCONTINUE,

	NUM_GAME_STATES						// PP: number of game states
};

enum ESubGameState{
	GAMESUBSTATE_NORMAL,
	GAMESUBSTATE_POSTCARD,
	GAMESUBSTATE_RUNON,					// TP: Taz runs into level
};

enum EGameMode{
	GAMEMODE_SINGLEPLAYER,
	GAMEMODE_MULTIPLAYER,
};

typedef struct TAG_CHARACTERSOUNDHANDLES			// structure to hold sound channels being used by characters
{
	int				taz;
	int				sheDevil;
	int				tweety;
	int				sam;
	int				daffy;
	int				tribal;
} CHARACTERSOUNDHANDLES, _CHARACTERSOUNDHANDLES;

// PP: player flags
enum EPlayerFlag
{
	PLAYERFLAG_DEFAULTCONTROL	=	(1<<0),						// PP: the player uses default control - this flag is cleared for some boss situations
};

// PP: default flags with which players are created
#define PLAYER_DEFAULT_FLAGS	PLAYERFLAG_DEFAULTCONTROL

typedef struct TAG_PLAYER
{
	struct TAG_ACTORINSTANCE	*actorInstance;					// TP: ActorInstance to use for this player
	struct TAG_CAMERAINFO		*camera;						// TP: Camera for this player to use
	struct TAG_PADCONTROLSTATUS	*controller;					// TP: Controller for player to take input from
	struct TAG_PLAYERSTATS		*stats;							// TP: Current stats of this player
	int							flags;							// PP: combination of player flags (EPlayerFlag)

} PLAYER, _PLAYER BALIGN16;

#define MAX_PLAYERS	(2)								// PP: was 4
#define MAX_SCREENS (2)								// PP: was 4

// PP: split screen modes
enum ESplitScreenMode
{
	SPLITSCREEN_HORIZONTAL =0,						// PP: horizontal split: player 1 on top screen, player 2 on bottom screen
	SPLITSCREEN_VERTICAL =1,						// PP: vertical split: player 1 on left screen, player 2 on right screen

}ENUMOPS(ESplitScreenMode);// PP: declare operator overloads

// PP: default split screen mode used when switching into 2 screens
#define DEFAULT_SPLIT_SCREEN_MODE					SPLITSCREEN_HORIZONTAL

enum ECustomOptions
{
	CUSTOMOPTION_DISABLECARTOONLIGHTING = (1<<0),
	CUSTOMOPTION_DISABLEOUTLINES = (1<<1),
};

enum EOutroState
{
	OUTRO_ENDCUTSCENE,
	OUTRO_STATS,
	OUTRO_NAMEENTRY,
	OUTRO_HIGHSCORES,
	OUTRO_INFORM,
	OUTRO_EXIT,
};

enum EIntroState
{
	INTRO_RULES,
	INTRO_CUTSCENE,
	INTRO_UPLOADCAMERACASESOUND,
	INTRO_CAMERACASES,
	INTRO_LOADSFX,
};


// PP: game status struct
typedef struct TAG_GAMESTATUS
{
	TBTimerValue	startTime,endTime,deltaTime;	// frame timers

	TBVector		windVelocity;					// global wind velocity, that affects things such as smoke and precipitation

	TBVector		viewPoint;						// sets current camera view point, used for culling
	TBVector		viewAng;						// PP: final-final angle vector of view

	int				currentState;					// current state, GAMESTATE_...
	int				currentSubState;				// current scene sub state, 
	int				currentScene;					// current scene, SCENE_PROTO etc..
	int				currentSceneType;				// current scene type, CUTSCENE etc..

	int				lastState;						// TP: Previous game state

	float			gameTime;						// current game time elapsed
	float			playTime;
	float			bestTime;						// best time for finishing demo
	struct TAG_ACTORINSTANCE	*player1;			// first player and single player structure
	struct TAG_ACTORINSTANCE	*player2;			// second player structure

	PLAYER			player[MAX_PLAYERS];			// TP: array of player structures

	SPLASHSCREEN	splashScreen;					// structure for controlling splash screen
	SPLASHSCREEN	pauseScreen;					// structure for controlling pause screen
	
// CPW: 	int				gameMode;						// current game mode SINGLE_PLAYER etc.
	bool			pad1Present;					// current presence of pads 0,1
	bool			pad2Present;
// PP: REMOUT: seems to do the opposite of pad1Present		bool			pad1Removed;					// JW: Controller 1 has been pulled out
// PP: REMOUT: seems to do the opposite of pad2Present		bool			pad2Removed;					// JW: Controller 2y has been pulled out

	char			controlSuspend;					// TRUE/FALSE taz control suspend
	char			suspendEnemies;					// TRUE/FALSE to freeze enemies
	char			suspendWorld;					// TRUE/FALSE to suspend world

	char			chrisCam;
	char			instanceName;					// TRUE/FALSE to show instance name
	char			instanceZone;					// TRUE/FALSE to show instance zones (Bounding box, axis, etc..)

	int				cheatsAvailable;				// PP: combination of all cheats currently available to the player - see Cheats.h
	float			appTime;						// TP: time since application was started
	GLOBALSETTINGS	globalSettings;					// PP: global settings adjustable from the front-end: volume levels, vibration, screen position etc. - see GlobalSettings.h

	int				quitNow;
#if(BPLATFORM==PC)
	int				noCDFound;						// NH: PC Only flag to determine if the game is quitting because the game CD was not found in and CD drive
#endif // NH: #if(BPLATFORM==PC)

	bool			wideScreenWilson;				// CPW: 
	bool			enableMusic;					// TP: Like it sounds

	float			gameSpeed;						// PP: game speed factor: 1.0f=normal, 0.5f=half speed, etc. // PP: This can be used for in-game speed-altering effects, independent of any cheats
	int				customOptions;					// TP: custom options for things like cartoon lighting etc. CUSTOMOPTION_...

	struct
	{
		int filterMode;								// CPW: texture filter mode to use BDRENDERSTATE_TEXTUREFILTER
		int pad[3];
	}gfxSettings;

	struct
	{
		char				numPlayers;
		char				playerPause;
		char				Tournament;
		char				versusCPU;			// CPW: if true, then play against CPU not player 2
		char				numScreens;
		int					numCheckPoints;
		ESplitScreenMode	splitScreenMode;		// PP: split-screen mode (horizontal/vertical) (SPLITSCREEN_...)
		char				currentScreen;			// PP: index of current screen being drawn
		char				countingDown;			
	}multiplayer;

	int64			frameNumber;					// PP: frame number: gets incremented each frame

	char			demoMode;						// JW: 0 = Normal play
													// JW: -1 = Recording Demo
													// JW: 1 = Playing Demo

	bool			navigatingMenus;				// PP: indicates that the player is navigating a menu - taz/camera controls are suspended during this time
	EOutroState		outroState;
	EIntroState		introState;
	static EBLanguageID	currentLanguage;				// TP: current language

}GAMESTATUS, _GAMESTATUS BALIGN16;

extern struct TAG_DISPLAYSETTINGS			videoMode;
extern float								fps;
extern float								speedRatio;
extern struct TAG_PADCONTROLSTATUS			controller1;
extern struct TAG_PADCONTROLSTATUS			controller2;
//extern struct TAG_CAMERAINFO				cameraInfo;
//extern struct TAG_CAMERAINFO				cameraInfo2;
extern char									commandLine[];
extern struct TAG_GAMESTATUS				gameStatus;
extern TBFont								*standardFont;
extern int									elevatorHandle;
extern struct TAG_PROFILE					profile;
extern struct TAG_PROFILE					tonyProfile;
extern TBStringTable						*mainStringTable;
extern char									*currentDate;
extern struct TAG_CHARACTERSOUNDHANDLES		characterSounds;
extern char									debugDPad;
#define GLOBAL2DVERTEXBUFFERSIZE			(4000)
extern TBPrimVertex2D						global2DVertexBuffer[GLOBAL2DVERTEXBUFFERSIZE];

// PP: when this is set to true, Babel has shutdown so DON'T TRY TO DELETE OR FREE ANYTHING!!!
extern bool									BabelHasShutdown;

#ifdef CONSUMERDEMO	// JW: consumer demo initialisation settings
	extern unsigned short language;
	extern unsigned short aspect;
	extern unsigned short playmode;
	extern unsigned short to_inactive;
	extern unsigned short to_gameplay;
#endif

// PP: update function type for the pause mode
typedef void (*PauseUpdateFunc)(const int32 context);

/* --------------------------------------------------------------------------------
   Function : InitGameState
   Purpose : Initialises the gameState structure
   Parameters : gameStatus
   Returns : 
   Info : 
*/
void InitGameState(GAMESTATUS *gameStatus);

/* --------------------------------------------------------------------------------
   Function : doCorporateStuff
   Purpose : play the opening corporate videos so the nasty lawyers don't sue us
   Parameters :
   Returns : 
   Info : 
*/

void doCorporateStuff();

/* --------------------------------------------------------------------------------
   Function : PrintDebugText
   Purpose : Outputs debug text to the screen
   Parameters : 
   Returns : 
   Info : 
*/
void PrintGameText(PLAYER *player);

/* --------------------------------------------------------------------------------
   Function		: DefaultPauseUpdateFunc
   Purpose		: update function used for default pause
   Parameters	: context value, required but currently not used
   Returns		: 
   Info			: // PP: this is the one that gets called as a result of setting GAMESTATE_PAUSE
					// PP: Must match the PauseUpdateFunc typedef (main.h)
*/
void DefaultPauseUpdateFunc(const int32 context);

/* --------------------------------------------------------------------------------
   Function		: DefaultPausePredrawFunc
   Purpose		: update function used for default pause
   Parameters	: context value, required but currently not used
   Returns		: 
   Info			: // PP: this is the one that gets called as a result of setting GAMESTATE_PAUSE
					// PP: Must match the PauseUpdateFunc typedef (main.h)
*/
void DefaultPausePredrawFunc(const int32 context);

/* --------------------------------------------------------------------------------
   Function : MainPause
   Purpose : main loop for pause mode
   Parameters : options - current main loop options ,(opt/default) ptr to update function used while paused,
				(opt/0) context value for update function used while paused
   Returns : 
   Info : // PP: added the updateFunc & updateFuncContext parameter to allow for custom pause modes
*/
void MainLoop(int options,PauseUpdateFunc updateFunc = NULL, const int32 updateFuncContext = 0, PauseUpdateFunc predrawUpdateFunc = NULL, const int32 predrawFuncContext=0);

/*  --------------------------------------------------------------------------------
	Function	: GameStateIntro
	Purpose		: Called during the intro phase of the level
	Parameters	: 
	Returns		: 
	Info		: 
*/
void GameStateIntro(void);

/* --------------------------------------------------------------------------------
   Function		: ApplySpeedAdjustments
   Purpose		: Effect any game-speed alterations, such as the TURBO and SLOWMO
					cheats and the global game-speed factor which can be used for
					special effects
   Parameters	: 
   Returns		: 
   Info			: 
*/
void ApplyGameSpeedAdjustments(void);

/* --------------------------------------------------------------------------------
   Function : MainObjectViewer
   Purpose : main loop for objectViewer mode within pause mode
   Parameters : 
   Returns : 
   Info : 
*/
void MainObjectViewer(void);

void UpdateInGameCameraCase();

void StartIngameCameraCase(int camCaseNum);

/* --------------------------------------------------------------------------------
   Function : MainIngame
   Purpose : main loop for ingame state
   Parameters : 
   Returns : 
   Info : 
*/
void MainIngame(void);

/* --------------------------------------------------------------------------------
   Function : SetGameState
   Purpose : sets the current game status
   Parameters : gameStatus, EGameState, EGameSubStatem,
				// PP: (opt/0) other numerical info (eg. pointer to update func for GAMESTATE_CUSTOMPAUSE),
				// PP: (opt/0) other numerical info (eg. context value for update func for GAMESTATE_CUSTOMPAUSE)
   Returns : 
   Info : 
*/
void SetGameState(GAMESTATUS *gameStatus, EGameState state, ESubGameState subState, const int32 parm1=0, const int32 parm2=0);

/* --------------------------------------------------------------------------------
   Function : SetGameSubState
   Purpose : sets the current game substatus
   Parameters : gameStatus, EGameState
   Returns : 
   Info : 
*/
void SetGameSubState(GAMESTATUS *gameStatus, ESubGameState subState);

/* --------------------------------------------------------------------------------
   Function : GetCurrentGameState
   Purpose : returns the current game status
   Parameters : GAMESTATUS
   Returns : 
   Info : 
*/
int GetCurrentGameState(GAMESTATUS *gameStatus);

/* --------------------------------------------------------------------------------
   Function : GetCurrentGameSubState
   Purpose : returns the current sub game status
   Parameters : GAMESTATUS
   Returns : 
   Info : 
*/
int GetCurrentGameSubState(GAMESTATUS *gameStatus);

/* --------------------------------------------------------------------------------
   Function : ChangeScene
   Purpose : destroys the current scene and loads another
   Parameters : GAMESTATUS,scene
   Returns : 
   Info : Must be careful where this is called, keep it at the end of the main loop
*/
int ChangeScene(GAMESTATUS *gameStatus, int scene);

/* --------------------------------------------------------------------------------
   Function : SetScene
   Purpose : Creates the first scene
   Parameters : GAMESTATUS, scene
   Returns : 
   Info : 
*/
int SetScene(GAMESTATUS *gameStatus, int scene);

/* --------------------------------------------------------------------------------
   Function : TakeScreenShot
   Purpose : Outputs screen shots to screen[number].bmp in exe directory
   Parameters : 
   Returns : 
   Info : 
*/
void TakeScreenShot(void);

/* --------------------------------------------------------------------------------
   Function : MainSplash
   Purpose : main loop for splash mode
   Parameters : 
   Returns : 
   Info : 
*/
void MainSplash(void);

/* --------------------------------------------------------------------------------
   Function : ProcessCommandLine
   Purpose : Parse the command line
   Parameters : 
   Returns : 
   Info : 
*/
void ProcessCommandLine(void);

/*  --------------------------------------------------------------------------------
	Function	: ProcessLevelOutro
	Purpose		: Handles the end of a level
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ProcessLevelOutro(void);

/*  --------------------------------------------------------------------------------
	Function	: Flip
	Purpose		: Called to flip the screen, and in the case of xbox render the consolewindow
	Parameters	: r,g,b,a,flags
	Returns		: 
	Info		: 
*/

void Flip(int red, int green, int blue, int alpha, int flags = BFLIPFLAG_UPDATE);

#endif