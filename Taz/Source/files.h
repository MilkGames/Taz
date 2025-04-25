#ifndef __H_FILES
#define __H_FILES

#define DATA_FILE_VERSION (float)18.0

extern int	bCellsDrawnPerc;
extern char	szProductLoc[80];

enum EPackageState
{
	PACKSTATE_NONE,
	PACKSTATE_AWAITINGLOAD,
	PACKSTATE_LOADING,
	PACKSTATE_READING,
	PACKSTATE_RESIDENT,
};

enum
{
	DRAW_SPLASH = 1<<0,
	DRAW_LEVELNAME = 1<<1,
	DRAW_MEMCARD = 1<<2,
	DRAW_FADE = 1<<3,
	DRAW_LOADBAR = 1<<4,
	DRAW_MEMCARDUPDATE = 1<<5,
	DRAW_MISSINGPAD = 1<<6,						// PP: draw missing-pad message
	DRAW_ALL = (-1) & ~DRAW_MISSINGPAD,			// PP: don't normally draw missing-pad message
};

typedef struct TAG_TAZCONTROL	// NH: The TAZCONTROL structure.  Holds the control data for the game
{
	int left;				// CONTROL: Control for Turn Left
	int right;				// CONTROL: Control for Turn Right
	int forward;			// CONTROL: Control for Move Forward
	int backward;			// CONTROL: Control for Move Backward
	int jump;				// CONTROL: Control for Jump
	int spin;				// CONTROL: Control for Spin
	int tiptoe;				// CONTROL: Control for TipToe
	int bite;				// CONTROL: Control for Bite
	int rant;				// CONTROL: Control for Rant (Special)
	int look;				// CONTROL: Control for Look Around (1st Person)
	int camLeft;			// CONTROL: Control for Rotate Camera Left
	int camRight;			// CONTROL: Control for Rotate Camera Right
	int pause;				// CONTROL: Control for Pause Game
	int select;				// CONTROL: Control for Select / Map
	int keyboardOffset;		// CONTROL: The offset for keyboard controls (take this from the keyboard control to get the DIK Code)
	BOOL forceFeedback;		// CONTROL: Force Feedback Enabled? // NH: No longer used
} TAZCONTROL, _TAZCONTROL;

typedef struct TAG_TAZPLAYER	// NH: The TazPlayer structure
{
#if BPLATFORM==PC
	GUID controllerGUID;		// CONTROL: The GUID of the controller
	TAZCONTROL keyboardData;	// CONTROL: Keyboard Data Structure
	TAZCONTROL joystickData;	// CONTROL: Joystick Data Structure
#endif
} TAZPLAYER, _TAZPLAYER;

typedef struct TAG_TAZSOUND		// NH: The TazSound structure
{
#if BPLATFORM==PC
	GUID		soundDriverGUID;	// SOUND: The GUID of the sound driver
#endif
} TAZSOUND, _TAZSOUND;

typedef struct TAG_TAZGRAPHICS		// NH: The TazGraphics structure
{
#if BPLATFORM==PC
	GUID		deviceIdentifier;	// GRAPHICS: The GUID of the graphics boards chipset
	int			width;				// GRAPHICS: Width of the specified mode
	int			height;				// GRAPHICS: Height of the specified mode
	int			zDepth;				// GRAPHICS: zDepth of the specified mode
	int			colours;			// GRAPHICS: No colours in the specified mode
	BOOL		windowed;			// GRAPHICS: If true use the current mode	
	BOOL		cartoonLighting;	// GRAPHICS: True if cartoon lighting enabled
	BOOL		cartoonOutline;		// GRAPHICS: True if cartoon outline enabled
	BOOL		doYouDoVoodoo;		// GRAPHICS: True if Voodoo campatability is enabled
	int			drawDist;			// GRAPHICS: The draw distance in meters (or is it metres?  I can never remember!)	
#endif
} TAZGRAPHICS, _TAZGRAPHICS;

//
typedef struct TAG_TAZDATA		// The TAZDATA structure to be read from file, and used to setup the game for PC
{
#if BPLATFORM==PC
	float		version;			// GENERAL: Version number of the Data File
	TAZSOUND	soundDetails;		// SOUND:	 Contains all the details about the selected sound settings
	TAZGRAPHICS	graphicsDetails;	// GRAPHICS: Contains all the details about the graphics setup
	TAZPLAYER	player1;			// CONTROL: Contains the control info for player 1
	TAZPLAYER	player2;			// CONTROL: Contains the control info for player 2
	int			language;			// LANGUAGE: The language specified for the launcher
#endif
} TAZDATA, _TAZDATA;


typedef struct TAG_PACKAGEENTRY
{
	char			pad[128];
	struct	TAG_PACKAGEENTRY	*next,*prev;
	uint32			nameCRC;
	TBPackageIndex	*index;
	char			name[128];
	char			actorToLoad[128];
	char			state;
	char			group;
	char			pad2[128];
} PACKAGEENTRY, _PACKAGEENTRY;

typedef struct TAG_PACKAGELIST
{
	PACKAGEENTRY	head;
	int				numEntries;
} PACKAGELIST, _PACKAGELIST;

extern TBPackageIndex		*masterPakfile;
extern TBPackageIndex		*splashPakfile;

extern TAG_TAZDATA			tazData;
extern BOOL					tazDataLoaded;
extern struct TAG_BOOK		*levelBook;				// PP: Book used to display the name of the level being loaded

/*  --------------------------------------------------------------------------------
	Function	: OpenPackage
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	OpenPackage(char *name);

/*  --------------------------------------------------------------------------------
	Function	: LoadPackage
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	LoadPackage(TBPackageIndex *parent, char *name);

/*  --------------------------------------------------------------------------------
	Function	: LoadPackageBkg
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	LoadPackageBkg(TBPackageIndex *parent, char *name, char *event, int *size);

/*	--------------------------------------------------------------------------------
	Function 	: InitFileSystem
	Purpose 	: Set up the file system and open the master pak file if it exists
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitFileSystem(void);

/*	--------------------------------------------------------------------------------
	Function 	: CloseFileSystem
	Purpose 	: Close the master pakfile
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CloseFileSystem(void);

/*	--------------------------------------------------------------------------------
	Function 	: CheckFileTag
	Purpose 	: Check a file for a given tag
	Parameters 	: filename,pakname,tag(FOURCC("SET ") etc) 
	Returns 	: 
	Info 		:
*/
int	CheckFileTag(char *filename, char *pakName, uint32 tag);

/*	--------------------------------------------------------------------------------
	Function 	: InitialisePackfileIndex
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: Called once at beginning of game
*/
void InitialisePackfileIndex(void);

/*	--------------------------------------------------------------------------------
	Function 	: AddPackfileToIndex
	Purpose 	: Add a pakfile to the linked list
	Parameters 	: pakIndex,name
	Returns 	: 
	Info 		: 
*/
PACKAGEENTRY *AddPackfileToIndex(TBPackageIndex *pakIndex, char *name);


/*	--------------------------------------------------------------------------------
	Function 	: RemovePackfileFromIndex
	Purpose 	: Remove a packfile from the index list
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/

void RemovePackfileFromIndex(char *name);


/*	--------------------------------------------------------------------------------
	Function 	: LoadWholePackage
	Purpose 	: Loads an entire package into memory
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
TBPackageIndex *LoadWholePackage(char *name,char group=-1);

/*	--------------------------------------------------------------------------------
	Function 	: FreeWholePackage
	Purpose 	: Frees a whole package file and all resorces within it
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
void FreeWholePackageGroup(char group);


/*	--------------------------------------------------------------------------------
	Function 	: FreeWholePackage
	Purpose 	: Frees a whole package file and all resorces within it
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
void FreeWholePackage(char *name);

/*	--------------------------------------------------------------------------------
	Function 	: FindPackage
	Purpose 	: Finds if a package is already loaded
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
TBPackageIndex	*FindPackage(char *name);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateIndex
	Purpose 	: Finds the package in memory and updates its pointer
	Parameters 	: name and new index
	Returns 	: 
	Info 		: This is needed on GameCube due to the way the bkFreePackageMemory 
				  changes the pointer to the package block
*/
void UpdateIndex(char *name, TBPackageIndex *newIndex);

/*	--------------------------------------------------------------------------------
	Function 	: PackageLoaded
	Purpose 	: Background loading is complete
	Parameters 	: The name of the event being triggered - this is case sensitive,
					the parameter string associated with the event,
					the data associated with the event,
					a client specified context value (set in bkTrapEvenCallback when callback was attached)
	Returns		: 
	Info		: // PP: must match the TBEventCallback typedef
*/
void PackageLoaded(char *name, char *parms, void* data, void *context);

/*	--------------------------------------------------------------------------------
	Function 	: GetPackageState
	Purpose 	: Get the current state of a package
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/

int	GetPackageState(char *name);

/* --------------------------------------------------------------------------------
   Function : GetCurrentBkgLoadingStatus
   Purpose : Returns TRUE if there is any background loading still in progress and performs GetPackageState commands
   Parameters : 
   Returns : TRUE if bkg loading is still taking place
   Info : 
*/
int	GetCurrentBkgLoadingStatus(void);


/*	--------------------------------------------------------------------------------
	Function 	: LoadWholePackageBkg
	Purpose 	: Loads an entire package into memory
	Parameters 	: name
	Returns 	: true/false if the load was started succesfully
	Info 		: 
*/

bool LoadWholePackageBkg(char *name,char *actorToLoad = NULL, char group = -1);


/* --------------------------------------------------------------------------------
   Function		: openLoadingBooks
   Purpose		: set up and open the books to be displayed during loading
   Parameters	: 
   Returns		: 
   Info			: // PP: call this right before looping drawLoadLoop.
*/
void openLoadingBooks(void);


/* --------------------------------------------------------------------------------
   Function		: closeLoadingBooks
   Purpose		: close the books that were displayed during loading
   Parameters	: 
   Returns		: 
   Info			: // PP: call this right after looping drawLoadLoop.
*/
void closeLoadingBooks(void);


/* --------------------------------------------------------------------------------
   Function : DrawLoadLoop
   Purpose : main loop for loading
   Parameters : 
   Returns : 
   Info : 
*/
void DrawLoadLoop(int drawFlags = DRAW_ALL);

/* --------------------------------------------------------------------------------
   Function : InitialiseLoadingBar
   Purpose : Reset the loading bar values
   Parameters : 
   Returns : 
   Info : 
*/
void InitialiseLoadingBar(int offset = 0);

/* --------------------------------------------------------------------------------
   Function : AddToLoadingBar
   Purpose : Increment the amount of bytes to be loaded
   Parameters : number of bytes to load
   Returns : 
   Info : 
*/
void AddToLoadingBar(int bytes);

/* --------------------------------------------------------------------------------
   Function : RemoveFromLoadingBar
   Purpose : Increment the amount of bytes that have been loaded
   Parameters : number of bytes loaded
   Returns : 
   Info : 
*/
void RemoveFromLoadingBar(int bytes);

/* --------------------------------------------------------------------------------
   Function : DrawLoadingBar
   Purpose : Draw the current state of the loading bar
   Parameters : 
   Returns : 
   Info : 
*/
void DrawLoadingBar(void);

/* --------------------------------------------------------------------------------
   Function : ProcessConfigFile
   Purpose : Loads the config file into a structure
   Parameters : 
   Returns : 
   Info : 
*/
void ProcessConfigFile(void);

/*  --------------------------------------------------------------------------------
	Function	: InitLoadBook
	Purpose		: Setup the load book used during drawloadloop
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitLoadBook(void);

/*  --------------------------------------------------------------------------------
	Function	: PackageSize
	Purpose		: Return the size of a package
	Parameters	: pakIndex or NULL, name
	Returns		: 
	Info		: 
*/
int32 PackageSize(TBPackageIndex *pakIndex, char *name);


#if BPLATFORM==PC
/*  --------------------------------------------------------------------------------
	Function	: InitMissingCdWarning
	Purpose		: 
	Parameters	: 
	Returns		:
	Info		: 
*/
void InitMissingCdWarning();

/*  --------------------------------------------------------------------------------
	Function	: CheckCD
	Purpose		: Checks the CD and displays/handles and warning messages
	Parameters	: 
	Returns		:
	Info		: 
*/

void CheckCD();

/*  --------------------------------------------------------------------------------
	Function	: ValidCD
	Purpose		: On the PC this checks that the game CD is in one of the drives
	Parameters	: 
	Returns		: true/false (CD in drive?)
	Info		: 
*/
bool ValidCD();

/*  --------------------------------------------------------------------------------
	Function	: CheckForOtherTazInstances
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

bool CheckForOtherTazInstances();

/*  --------------------------------------------------------------------------------
	Function	: DrawMouse
	Purpose		: like it says
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawMouse();

#endif

/*  --------------------------------------------------------------------------------
	Function	: AppendCountryCode
	Purpose		: Append the correct country code ( _uk etc) to the name. used for loading samples and other localised files
	Parameters	: nameIn, nameOut
	Returns		: 
	Info		: WARNING - make sure there is enough room in nameOut for the extra characters
*/
char *AppendCountryCode(char *nameIn, char *nameOut);

/*  --------------------------------------------------------------------------------
	Function	: LoadTazDataPC
	Purpose		: Use the taz config to setup to controllers etc
	Parameters	: 
	Returns		: 
	Info		: 
*/
#if BPLATFORM==PC
void LoadTazDataPC();
#endif


/*  --------------------------------------------------------------------------------
	Function	: PrintPackageStates
	Purpose		: Prints the state of all packages in memory
	Parameters	: 
	Returns		: 
	Info		: 
*/

void PrintPackageStates(void);


/*	--------------------------------------------------------------------------------
	Function 	: badDisk
	Purpose 	: handle a fatal disk read error
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this has to interrupt whatever loading was going on at the time
					// PP: I kept the name short because I can see us using this function a lot.
*/
void badDisk(void);

/*  --------------------------------------------------------------------------------
	Function	: BytesOnLoadingBar
	Purpose		: Returns the number of bytes on the loading bar
	Parameters	: 
	Returns		: number of bytes
	Info		: 
*/

int BytesOnLoadingBar(void);


#endif