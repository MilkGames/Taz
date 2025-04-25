#ifndef __H_CUTSCENE
#define __H_CUTSCENE

#include "actors.h"
#include "linkedlist.h"
#include "display.h"

// PP: are we playing an intro cutscene the noo
#define PLAYING_INTRO_CUTSCENE		((GetCurrentGameState(&gameStatus)==GAMESTATE_INTRO)&&(gameStatus.introState==INTRO_CUTSCENE))

// PP: are we playing an outro cutscene the noo
#define PLAYING_OUTRO_CUTSCENE		((GetCurrentGameState(&gameStatus)==GAMESTATE_OUTRO)&&(gameStatus.outroState==OUTRO_ENDCUTSCENE))

// PP: are we playing a cutscene of some sort the noo
#define PLAYING_CUTSCENE			(PLAYING_INTRO_CUTSCENE||PLAYING_OUTRO_CUTSCENE)

// PP: ~~~~~~~ use CUTSCENE_SKIP to determine if the player is pressing a button that should skip a cutscene ~~~~~~

#if(NUM_CONTROLLER_PORTS == 4)
#define OR_MAYBE_A_BUTTON_ON_CONTROLLER_3_OR_4\
		||	(controller3.crossDebounceChannel->value)\
		||	(controller4.crossDebounceChannel->value)

#define OR_MAYBE_START_BUTTON_ON_CONTROLLER_3_OR_4\
		||	(controller3.startDebounceChannel->value)\
		||	(controller4.startDebounceChannel->value)
#else// PP: 2 ports
#define OR_MAYBE_START_BUTTON_ON_CONTROLLER_3_OR_4
#endif// PP: 2 ports

#if(BPLATFORM == PC)
#define	OR_MAYBE_A_PC_SPECIFIC_SKIP_BUTTON\
		|| (controller1.menuSelectDebounceChannel->value)\
		|| (controller1.crossDebounceChannel->value)\
		|| (controller1.mb1DebounceChannel->value)
#else// PP: !pc
#define OR_MAYBE_A_PC_SPECIFIC_SKIP_BUTTON
#endif// PP: !pc

// PP: CUTSCENE_SKIP evaluates to non-zero if the player is pressing a button that should skip the cutscene
#if BPLATFORM==XBOX
#define	CUTSCENE_SKIP\
		((controller1.startDebounceChannel->value||controller1.crossDebounceChannel->value)\
		/* PP: we must allow ANY controller to skip a cutscene if the pad-checking mode is PADCHECK_ALL*/\
		||	((padCheckMode == PADCHECK_ALL)\
			&&	(	(controller2.startDebounceChannel->value||controller1.crossDebounceChannel->value)\
					OR_MAYBE_START_BUTTON_ON_CONTROLLER_3_OR_4\
					OR_MAYBE_A_BUTTON_ON_CONTROLLER_3_OR_4\
				)\
			)\
		OR_MAYBE_A_PC_SPECIFIC_SKIP_BUTTON\
		)
#else
#define CUTSCENE_SKIP\
		((controller1.startDebounceChannel->value)\
		/* PP: we must allow ANY controller to skip a cutscene if the pad-checking mode is PADCHECK_ALL*/\
		||	((padCheckMode == PADCHECK_ALL)\
			&&	(	(controller2.startDebounceChannel->value)\
					OR_MAYBE_START_BUTTON_ON_CONTROLLER_3_OR_4\
				)\
			)\
		OR_MAYBE_A_PC_SPECIFIC_SKIP_BUTTON\
		)
#endif

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum ECutsceneFlags
{
	CUTSCENEFLAG_AUTOLOAD = 1<<0,			// TP: Sets the cutscene to auto load and play any objects in pakfile
	CUTSCENEFLAG_PLACEHOLDER = 1<<1,		// TP: cutscene not finished so display placeholder message
	CUTSCENEFLAG_FADEOUT = 1<<2,			// TP: cutscene should fade out at end
	CUTSCENEFLAG_SKIP = 1<<3,				// TP: player has chosen to skip cutscene
};

enum EEntryType
{
	ADDNORMALOBJECT,
	ADDCARTOONOBJECT,
	ADDLINEOBJECT,
	ADDINVISIBLEOBJECT,
	REMOVEOBJECT,
	HIDEOBJECT,
	HIDEOBJECTPERM,				// TP: hide an object permanently
	UNHIDEOBJECT,
	TRACKBONE,
};

enum ECutsceneState
{
	CUTSCENE_INACTIVE,			// TP: cutscene is dormant and no objects loaded
	CUTSCENE_LOADED,			// TP: cutscene pak has been loaded
	CUTSCENE_ACTIVE,			// TP: cutscene objects have been created
	CUTSCENE_READY,				// TP: cutscene is ready to play, just awaiting sync with music etc
	CUTSCENE_PLAYING,			// TP: cutscene is currently playing
};

typedef struct TAG_CUTSCENEENTRY
{

	char							actor[64];				// TP: name of actor to create or instance to remove
	uint32							crc;					// TP: crc if there is no name
	struct TAG_ACTORINSTANCE		*actorInstance;			// TP: pointer to actor instance when created
	EEntryType						type;					// TP: type of item

} CUTSCENEENTRY, _CUTSCENEENTRY BALIGN16;

typedef int (*Callback)(class TAG_CUTSCENE *cutscene);

typedef class TAG_CUTSCENE
{
private:
	LINKEDLIST<CUTSCENEENTRY>	entries;		// TP: list of items to process before starting cutscene
	CAMERAINFO					saveCamera;		// TP: copy of previous camera
	CAMERAINFO					*camera;		// TP: camera to apply the cutscene to
	ACTORINSTANCE				*camLookat;		// TP: object for camera to lookat
	ACTORINSTANCE				*camEye;		// TP: object for camera to follow
	ECutsceneState				state;			// TP: current state of cutscene
	char						objectSourceDir[128];	// TP: directory to get new objects from
	char						soundSourceDir[128];	// TP: sound source
	int							flags;			// TP: Any cutscene flags
	TBPackageIndex				*pakIndex;		// TP: pakindex of cutscene package
	TBPackageIndex				*soundPakIndex;	// TP: pakindex of the cutscene sound
	char						spinObe[64];	// TP: node the spin model should be attached to
	char						spinNode[64];	// TP: node the spin model should be attached to
	TAG_CUTSCENEENTRY			*spinEntry;
	
	char						tazObe[64];
	char						tazNode[64];
	char						music[64];		// TP: name of music track to play with cutscene
	TAG_CUTSCENEENTRY			*tazEntry;
	TBVector					tazPos;
	TBActorNodeInstance			*taznodeInstance;

	float						oldFarPlane;
	float						farPlane;
	
	bool						fog;			// TP: Should fog be on or off for this cutscene
	bool						currentFogState;// TP: fog state before the cutscene was played
	
	static ACTORINSTANCE		*spinInstance;	// TP: spin model

	struct
	{
		char						texName[64];
		TBTexture					*texture;	// TP: any overlay for the cutscene
		float						x,y;
	} overlay;

	static	bool				overlayOn;

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Init
		Purpose		: Create a cutscene
		Parameters	: 
		Returns		: TRUE/FALSE
		Info		: 
	*/
	int Init(char *objectSource, int flags = 0);

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::StartAnimations
		Purpose		: Start all the cutscene objects animating
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	void StartAnimations();

public:

	Callback					SetupCallback;

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::TAG_CUTSCENE
		Purpose		: Default constructor
		Parameters	: source directory in cutscene pakfile for objects
		Returns		: 
		Info		: 
	*/
	TAG_CUTSCENE(char *objectSource, int flags = 0)
	{
		Init(objectSource,flags);
	}

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::~TAG_CUTSCENE
		Purpose		: Default destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_CUTSCENE();

	/*  --------------------------------------------------------------------------------
		Function	: Fog
		Purpose		: enable or disable fog for this cutscene
		Parameters	: state
		Returns		: 
		Info		: 
	*/
	void Fog(bool state)
	{
		fog = state;
	}


	/*  --------------------------------------------------------------------------------
		Function	: FarPlane
		Purpose		: Set the far plane to use for this cutscene
		Parameters	: distance
		Returns		: 
		Info		: 
	*/

	void FarPlane(float val)
	{
		oldFarPlane = videoMode.farPlane;
		farPlane = val;
	}


	/*  --------------------------------------------------------------------------------
		Function	: FadeOut
		Purpose		: should the cutscene fade out towards the end
		Parameters	: true/false
		Returns		: 
		Info		: 
	*/

	void FadeOut(bool state)
	{
		if (state)
		{
			flags |= CUTSCENEFLAG_FADEOUT;
		}
		else
		{
			flags &= ~CUTSCENEFLAG_FADEOUT;
		}
	}

	/*  --------------------------------------------------------------------------------
		Function	: FadeUp
		Purpose		: Start the screen fading up
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	void FadeUp()
	{
		camera->fade.StartFadeUp(FADETYPE_NORMAL, 1.0f);
	}


	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::State
		Purpose		: Return the current state of the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	ECutsceneState State()
	{
		if (state==CUTSCENE_READY) return CUTSCENE_PLAYING;
		return state;
	}

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Setup
		Purpose		: Create and remove all entries in the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Setup();

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::AddEntry
		Purpose		: Add an entry to the cutscene entry list
		Parameters	: type of entry, name of item to add or remove
		Returns		: 
		Info		: 
	*/
	void AddEntry(EEntryType type, char *name);

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::AddEntry
		Purpose		: Add an entry to the cutscene entry list
		Parameters	: type of entry, crc
		Returns		: 
		Info		: 
	*/
	void AddEntry(EEntryType type, uint32 crc);

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Load
		Purpose		: Load create and remove all entries in the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	bool Load();

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::FinishedLoading
		Purpose		: Test if the cutscene has finished loading
		Parameters	: 
		Returns		: TRUE if finished
		Info		: 
	*/
	int FinishedLoading();

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::FinishedPlaying
		Purpose		: Test if the cutscene has finished playing
		Parameters	: 
		Returns		: TRUE if finished
		Info		: 
	*/
	int FinishedPlaying();

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Play
		Purpose		: Start playing the cutscene
		Parameters	: camera to apply the cutscene to
		Returns		: 
		Info		: 
	*/
	void Play(struct TAG_CAMERAINFO *camera);

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Stop
		Purpose		: Stop playing the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Stop();

	/*  --------------------------------------------------------------------------------
		Function	: CUTSCENE::Skip
		Purpose		: Cause the cutscene to finish early
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	void Skip()
	{
		flags |= CUTSCENEFLAG_SKIP;

		if(strcmpi(objectSourceDir, "cityhub_intro") == 0)
		{
			bkGenerateEvent("trigger", "_activate=fountainvan");
		}
	}

	/*  --------------------------------------------------------------------------------
		Function	: GetFlags
		Purpose		: Return the current flags using in the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	int GetFlags(void)
	{
		return flags;
	}

	/*  --------------------------------------------------------------------------------
		Function	: Update
		Purpose		: Updates the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Update(void);

	/*  --------------------------------------------------------------------------------
		Function	: Draw
		Purpose		: Draw any extras for the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Draw(void);

	/*  --------------------------------------------------------------------------------
		Function	: Draw2
		Purpose		: Draw any extras for the cutscene
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Draw2(void);

	/*  --------------------------------------------------------------------------------
		Function	: SetSpinNode
		Purpose		: Sets the spin node to be used
		Parameters	: obe name, node name
		Returns		: 
		Info		: 
	*/
	void SetSpinNode(char *obe,char *node);

	/*  --------------------------------------------------------------------------------
		Function	: SetTazNode
		Purpose		: Sets the taz root node to be used
		Parameters	: obe name, node name
		Returns		: 
		Info		: 
	*/
	void SetTazNode(char *obe,char *node);

	/*  --------------------------------------------------------------------------------
		Function	: SetMusic
		Purpose		: Sets the music that should be played during the cutscene
		Parameters	: music track name
		Returns		: 
		Info		: 
	*/
	void SetMusic(char *music)
	{
		strcpy(this->music, music);
	}


	/*  --------------------------------------------------------------------------------
		Function	: SetOverlay
		Purpose		: Specify a texture to be draw over the cutscene
		Parameters	: texture, pos x, pos y
		Returns		: 
		Info		: 
	*/

	void SetOverlay(char *name, float x, float y)
	{
		strcpy(overlay.texName, name);
		overlay.texture = NULL;
		overlay.x = x;
		overlay.y = y;
	}

	/*  --------------------------------------------------------------------------------
		Function	: CutsceneTv
		Purpose		: Turns the sam cutscene 
		Parameters	: true/false
		Returns		: 
		Info		: 
	*/

	friend void CutsceneTv(bool state);

} CUTSCENE, _CUTSCENE BALIGN16;



/*  --------------------------------------------------------------------------------
	Function	: HideActorInstanceList
	Purpose		: Hide all actors on the given instance list
	Parameters	: instance list
	Returns		: 
	Info		: 
*/

void HideActorInstanceList(ACTORINSTANCELIST *actorInstanceList);

/*  --------------------------------------------------------------------------------
	Function	: LoadLevelCutscene
	Purpose		: If the level needs a cutscene to play on starting then this function will start a background load
					if the pakfile and setup the cutscene ready to play.
	Parameters	: sceneNumber
	Returns		: CUTSCENE or NULL
	Info		: 
*/
CUTSCENE* LoadLevelCutScene(int sceneNumber);

/*  --------------------------------------------------------------------------------
	Function	: SetupHub
	Purpose		: This function is called to setup the special hub objects based on which levels have been completed
	Parameters	: sceneNumber
	Returns		: 
	Info		: 
*/
void SetupHub(int sceneNumber);

/* --------------------------------------------------------------------------------
   Function : DrawPlaceHolderLoop
   Purpose : Display message for 3 seconds, allow skip using x button
   Parameters : 
   Returns : 
   Info : 
*/
void DrawPlaceHolderLoop(char *text);


/* --------------------------------------------------------------------------------
   Function : ChooseMusicForOutroCutscene
   Purpose : Choose the correct music for the exit cutscene
   Parameters : cutscene
   Returns : 
   Info : 
*/
void ChooseMusicForOutroCutscene(CUTSCENE *cutscene);

#endif