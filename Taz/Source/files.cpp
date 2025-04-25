// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : files.cpp
//   Purpose : provide support functions for actor and texture loading
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "files.h"
#include "parsemap.h"
#include "fade.h"
#include "icon.h"
#include "main.h"
#include "control.h"
#include "display.h"
#include "timer.h"
#include "music.h"
#include "MemCard.h"			// PP: memory card operations and dialogues
#include "camera.h"
#include "scene.h"
#include "mapfile.h"
#include "events.h"
#include "gui.h"
#include "PageItemFX.h"			// PP: Special effects for page items

#if BPLATFORM == PC
	#include "Windows.h"
	#define CD_VOLUME_NAME "TazWanted"
#ifndef CONSUMERDEMO

#ifdef _MASTER
	#define REG_KEY "Software\\Infogrames\\TazWanted\\Release"
#endif // NH: #ifdef _MASTER

#ifndef _MASTER
	#define REG_KEY "Software\\Infogrames\\TazWanted\\Pre-release"
#endif // NH: #ifndef _MASTER

#else // NH: #ifndef CONSUMERDEMO
	#define REG_KEY "Software\\Infogrames\\TazWanted Demo\\Demo"
#endif // NH: #ifndef CONSUMERDEMO (else)

#endif // NH: #if BPLATFORM == PC

#if(BPLATFORM == XBOX)
#ifdef CONSUMERDEMO
// PP: debug package loads/opens etc.
#define FILES_DEBUG
#endif// PP: def CONSUMERDEMO
#endif// PP: xbox

#ifdef CONSUMERDEMO
	#define LOADINGBARX	(0.125f)
	#define LOADINGBARY (-0.81f)

	#define TOURLOADINGBARX	(-0.34f)
	#define TOURLOADINGBARY (-0.675f)
#else
	#define LOADINGBARX	(0.0625f)
	#define LOADINGBARY (0.535f)

	#define TOURLOADINGBARX	(-0.34f)
	#define TOURLOADINGBARY (-0.675f)
#endif

#define LOADINGBARW (0.328125f)
#define LOADINGBARH (0.0669642857142857142857142857142857f)

PACKAGELIST	packageList;

TBPackageIndex		*masterPakfile;
TBPackageIndex		*splashPakfile;
char				splashFilename[260];


typedef struct TAG_LOADINGBAR
{
	int		totalBytesToLoad;
	int		bytesLoaded;
} LOADINGBAR;

static LOADINGBAR	loadingBar;
TAZDATA				tazData;
BOOL				tazDataLoaded = FALSE;

BOOK				*levelBook;		// PP: Book used to display the name of the level being loaded
PAGE				*levelPage;
TEXTBOX				*levelName;
TEXTBOX				*loadingText;

#ifdef CONSUMERDEMO
BOOK				*demoObjsBook;
#endif

char			szProductLoc[80];

TBEventClient		*loadEventClient;

#if BPLATFORM == PC
static GUID			nullGuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };	// NULL guid

BOOK				missingCdBook;
#endif

volatile uint32		vsyncCount;

/*  --------------------------------------------------------------------------------
	Function	: VsyncCallback
	Purpose		: Called every frame to update vsyncCount
	Parameters	: 
	Returns		: 
	Info		: 
*/

void VsyncCallback(void *context)
{
	vsyncCount++;
}


/*  --------------------------------------------------------------------------------
	Function	: WaitForVSync
	Purpose		: Wait for the next vsync
	Parameters	: none
	Returns		: 
	Info		: 
*/

void WaitForVSync(void)
{
	uint32	vsync;

	vsync = vsyncCount;
	while(vsync == vsyncCount)
	{
		bkSleep(0);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: OpenPackage
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	OpenPackage(char *name)
{
	TBPackageIndex	*pakIndex=NULL;

#ifdef FILES_DEBUG
	bkPrintf("OpenPackage: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

	pakIndex=bkOpenPackage(name);

	return pakIndex;
}

/*  --------------------------------------------------------------------------------
	Function	: LoadPackage
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	LoadPackage(TBPackageIndex *parent, char *name)
{
	TBPackageIndex	*pakIndex=NULL;

#ifdef FILES_DEBUG
	bkPrintf("LoadPackage: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

	pakIndex=bkLoadPackage(parent,name);

	return pakIndex;
}

/*  --------------------------------------------------------------------------------
	Function	: LoadPackageBkg
	Purpose		: try opening a package using some different search paths (not supported by babel on ps2 at moment)
	Parameters	: name
	Returns		: 
	Info		: 
*/
TBPackageIndex*	LoadPackageBkg(TBPackageIndex *parent, char *name, char *event, int *size)
{
	TBPackageIndex	*pakIndex=NULL;
	char			tempChar[256];
	
#ifdef FILES_DEBUG
	bkPrintf("LoadPackageBkg: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

	pakIndex=bkLoadPackageBkg(parent,name,event,size);

	if (!pakIndex)
	{
		sprintf(tempChar,"..\\paks\\%s",name);
		if (!(pakIndex=bkLoadPackageBkg(parent,tempChar,event,size)))
		{
			sprintf(tempChar,"paks\\%s",name);
			pakIndex=bkLoadPackageBkg(parent,tempChar,event,size);
		}
	}

	// PP: if anything went wrong, let's call it a bad disk
	if(pakIndex == NULL)
	{
		badDisk();
	}

	return pakIndex;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitFileSystem
	Purpose 	: Set up the file system and open the master pak file if it exists
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitFileSystem(void)
{
#if BPLATFORM==PS2

	// TP: bdSetVsyncCallback(VsyncCallback, NULL);

	#ifdef CONSUMERDEMO
		#ifdef USE_HOST0
			masterPakfile = bkOpenPackage("taz");
			splashPakfile = bkOpenPackage("splshscr");
			sprintf(splashFilename, "splshscr");
		#else
			#ifndef STANDALONE
				masterPakfile = bkOpenPackage("tazdemo\\taz");
				splashPakfile = bkOpenPackage("tazdemo\\splshscr");
				sprintf(splashFilename, "tazdemo\\splshscr");
			#else
				masterPakfile = bkOpenPackage("taz");
				splashPakfile = bkOpenPackage("splshscr");
				sprintf(splashFilename, "splshscr");
			#endif
		#endif
	#else
		masterPakfile = bkOpenPackage("taz");
		splashPakfile = bkOpenPackage("splshscr");
		sprintf(splashFilename, "splshscr");
	#endif
#else
	masterPakfile = NULL;
	splashPakfile = NULL;
	sprintf(splashFilename, "");
#endif

	bkSetFileSearchPath(FILE_SEARCHPATHFLAGS_DEFAULTPATHFIRST,2,"..\\paks","paks\\");

	if (bkCreateEvent("load") == FAIL)	bkPrintf("*** ERROR *** Cannot setup load event.\n");
	loadEventClient = bkTrapEventCallback("load", PackageLoaded, 0);
}

/*  --------------------------------------------------------------------------------
	Function	: InitLoadBook
	Purpose		: Setup the
	load book used during drawloadloop
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitLoadBook(void)
{
	float	x;
	
	PAGE* page;

	levelBook = new BOOK();

	levelBook->setEffectChooser(NULL);
	
	levelBook->setName("levelBook");

	// PP: NEW: as with any book that has to line up with a splash screen, we've got to ignore the 'safe area' otherwise things'll shift about between platforms
	levelBook->flags &= ~BKFLAG_SAFEAREA;

	levelPage = levelBook->addPage("level");

	//x = LOADINGBARX / 320.0f;	

	levelPage->insertFreeItem(levelName = new TEXTBOX(),RECTANGLE(-0.35f, 0.45f, -0.45f, -0.3f));

#ifndef CONSUMERDEMO
	levelPage->insertFreeItem(loadingText = new TEXTBOX(STR_LOADING),RECTANGLE(-0.35f, 0.45f, -0.45f, -0.3f));
	loadingText->setEffect(&PIE_fade);
	loadingText->setXAlign(PIFLAG_XALIGN_LEFT);
	loadingText->setFont(standardFont);
	loadingText->setWrap(false);

	loadingText->setFontSize(0.55f);
	loadingText->setColour(COLOUR(95,95,95),COLOUR(95,95,95));
#endif

	levelName->setEffect(&PIE_fade);// PP: can't use a fancy effect on this because it's displayed during loads

	// PP: with any luck all platforms should use effectively the same splashes and the levelname positions now
	// PP: REMOUT: ooooh, this free rect IS NOT WORKING!!!!! // PP: TODO: FIND OUT WHY!!!!!!			levelPage->insertFreeItem(levelName = new TEXTBOX(),RECTANGLE(-0.35f, 0.45f, 0.0f, 0.5f));
	
	// PP: TEMP TEST
	// PP: leave in		levelPage->setBackgroundColour(COLOUR(0,255,0,70));
	// PP: leave in		levelName->setBoxStyle(false, false);
	// PP: leave in		levelName->setBackgroundColour(COLOUR(255,0,0,70));

	// PP: REMOUT: hmm not yet, see above		levelName->setYAlign(PIFLAG_YALIGN_BOTTOM);// PP: UNREMOUT(PP)

	levelName->setFont(standardFont);


	levelName->setColour(COLOUR(95,95,95),COLOUR(95,95,95));

#ifdef CONSUMERDEMO

	#define OBJECTIVE_FONTSIZE				0.7f// PP: JC commented that text was a bit small	0.6f

// PP: REMOUT: using a text style now	#define OBJECTIVE_HEADING_FONTSIZE		0.9f// PP: heading font size

	PAGE	*subPage;
	PAGE	*subsubPage;
	ICON	*icon;
	TEXTBOX	*text;

	demoObjsBook = new BOOK(); 

	demoObjsBook->setEffectChooser(NULL);

	page = demoObjsBook->addPage("single objectives");

	page->insertFreeItem(subPage = new PAGE("objs"), RECTANGLE(-0.00f, 0.50f, -0.28f, 0.48f));

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_DEPTSTORE_POSTER_HEADING));
	text->setStyle(TS_smallTitle);// PP: 
	text->setScale(OBJECTIVE_FONTSIZE);
	text->setWrap(true);

#define DEMO_OBJECTIVES(_string) \
{\
	subPage->insertItem(subsubPage = new PAGE()); \
	\
	subsubPage->insertItem(icon = new ICON("redbutton.bmp")); \
	\
	icon->setXPadding(false); \
	icon->tilt=false;/* PP: */ \
	icon->setYAlign(PIFLAG_YALIGN_TOP); \
	icon->setAlignScale(0.31f); \
	icon->setDrawScale(0.22f); \
	\
	subsubPage->insertItem(text = new TEXTBOX(_string), false); \
	text->setFontSize(OBJECTIVE_FONTSIZE); \
	text->setWrap(true); \
	text->setXAlign(PIFLAG_XALIGN_LEFT); \
	text->setYAlign(PIFLAG_YALIGN_TOP);/* PP: */ \
}

	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE1);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE2);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE3);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE4);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE5);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE6);
	DEMO_OBJECTIVES(STR_DEMO_DEPTSTORE_POSTEROBJECTIVE7);

	/*subPage->insertItem(text = new TEXTBOX(STR_DEMO_DEPTSTORE_HELPTEXT1));
	text->setFontSize(OBJECTIVE_FONTSIZE);
	text->setWrap(true);

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_DEPTSTORE_HELPTEXT2));
	text->setFontSize(OBJECTIVE_FONTSIZE);
	text->setWrap(true);*/

	page = demoObjsBook->addPage("multi objectives");

	page->insertFreeItem(subPage = new PAGE("objs"), RECTANGLE(0.0f, 0.50f, -0.28f, 0.48f));

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_DEPTSTORE_TROLLEY_HEADING));
	text->setStyle(TS_smallTitle);// PP: 
	text->setScale(OBJECTIVE_FONTSIZE);// PP: REMOUT: now matches up with the singleplayer heading	// PP: text->setScale(0.68f);
	text->setWrap(true);
	/*
	subPage->insertItem(text = new TEXTBOX(STR_DEMO_DEPTSTORE_TROLLEYOBJECTIVE));
	text->setScale(0.68f);
	text->setWrap(true);*/

	TEXTBOX		*tb1, *tb2;

#if(BPLATFORM == PC)
	subPage->insertItem(tb1 = new TEXTBOX(STR_PC_DEMO_DEPTSTORE_TROLLEYOBJECTIVE_1));
	subPage->insertItem(tb2 = new TEXTBOX(STR_PC_DEMO_DEPTSTORE_TROLLEYOBJECTIVE_2));
#elif(BPLATFORM == XBOX)
	subPage->insertItem(tb1 = new TEXTBOX(STR_XBOX_DEMO_DEPTSTORE_TROLLEYOBJECTIVE1));
	subPage->insertItem(tb2 = new TEXTBOX(STR_XBOX_DEMO_DEPTSTORE_TROLLEYOBJECTIVE2));
#else
	subPage->insertItem(tb1 = new TEXTBOX(STR_DEMO_DEPTSTORE_TROLLEYOBJECTIVE1));
	subPage->insertItem(tb2 = new TEXTBOX(STR_DEMO_DEPTSTORE_TROLLEYOBJECTIVE2));
#endif // NH: BPLATFORM

	tb1->setFontSize(OBJECTIVE_FONTSIZE);
	tb1->setWrap(true);
	
	tb2->setFontSize(OBJECTIVE_FONTSIZE);
	tb2->setWrap(true);

#endif
}

/*	--------------------------------------------------------------------------------
	Function 	: CloseFileSystem
	Purpose 	: Close the master pakfile
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CloseFileSystem(void)
{
	// PP: must explicitly shut-down the book; seems that otherwise it gets destroyed far too late
	levelBook->shutdown();

	delete(levelBook);

#ifdef CONSUMERDEMO
	demoObjsBook->shutdown();
	delete(demoObjsBook);
#endif

	if (masterPakfile)	bkClosePackage(masterPakfile);
	if (splashPakfile) bkClosePackage(splashPakfile);

	bkDeleteEventClient(loadEventClient);
	bkDeleteEvent("load");
}

/*	--------------------------------------------------------------------------------
	Function 	: CheckFileTag
	Purpose 	: Check a file for a given tag
	Parameters 	: filename,pakname,tag(FOURCC("SET ") etc) 
	Returns 	: 
	Info 		:
*/
int	CheckFileTag(char *filename, char *pakName, uint32 tag)
{
	TBPackageIndex	*pakIndex;
	TBFileTagInfo	tagInfo;
	int				tagID;

	if ((pakIndex = bkOpenPackage(pakName)))
	{
		bkGetPakFileTags(pakIndex,filename,&tagInfo);
		bkClosePackage(pakIndex);
		
		for(tagID = 0; tagID < tagInfo.noofTags; tagID++)
		{
			if (tagInfo.tags[tagID] == tag) return TRUE;
		}

	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitialisePackfileIndex
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: Called once at beginning of game
*/
void InitialisePackfileIndex(void)
{
	packageList.numEntries = 0;
	packageList.head.next = packageList.head.prev = &packageList.head;

}

/*	--------------------------------------------------------------------------------
	Function 	: AddPackfileToIndex
	Purpose 	: Add a pakfile to the linked list
	Parameters 	: pakIndex,name
	Returns 	: 
	Info 		: 
*/
PACKAGEENTRY *AddPackfileToIndex(TBPackageIndex *pakIndex, char *name)
{
	PACKAGEENTRY	*ptr,*packageListEntry;

	ptr = packageList.head.next;

	packageListEntry = (PACKAGEENTRY*)ZALLOC(sizeof(PACKAGEENTRY));
	ASSERT(packageListEntry);

	packageListEntry->next = ptr;
	packageListEntry->prev = ptr->prev;
	ptr->prev->next = packageListEntry;
	ptr->prev = packageListEntry;
	packageList.numEntries++;

	strcpy(packageListEntry->name,name);

	packageListEntry->index = pakIndex;
	packageListEntry->nameCRC = bkCRC32((uchar*)packageListEntry->name,strlen(packageListEntry->name),0);
	packageListEntry->state = PACKSTATE_LOADING;
	packageListEntry->group = -1;
	return packageListEntry;
}

/*	--------------------------------------------------------------------------------
	Function 	: RemovePackfileFromIndex
	Purpose 	: Remove a packfile from the index list
	Parameters 	: pakIndex,name
	Returns 	: 
	Info 		: 
*/
void RemovePackfileFromIndex(char *name)
{
	PACKAGEENTRY	*ptr;
	uint32			crc;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->nameCRC == crc)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			packageList.numEntries--;
			SAFE_FREE(ptr);
			return;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: AppendCountryCode
	Purpose		: Append the correct country code ( _uk etc) to the name. used for loading samples and other localised files
	Parameters	: nameIn, nameOut
	Returns		: nameOut
	Info		: WARNING - make sure there is enough room in nameOut for the extra characters
*/
char* AppendCountryCode(char *nameOut, char *nameIn)
{
	switch(gameStatus.currentLanguage)
	{
	case BLANGUAGEID_UK:
		sprintf(nameOut, "%s_uk", nameIn);
		break;

	case BLANGUAGEID_D:
		sprintf(nameOut, "%s_d", nameIn);
		break;

	case BLANGUAGEID_F:
		sprintf(nameOut, "%s_f", nameIn);
		break;

	case BLANGUAGEID_E:
		sprintf(nameOut, "%s_e", nameIn);
		break;

	case BLANGUAGEID_IT:
		sprintf(nameOut, "%s_it", nameIn);
		break;
	}

	return nameOut;
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadWholePackage
	Purpose 	: Loads an entire package into memory
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
TBPackageIndex *LoadWholePackage(char *name,char group)
{
#ifdef FILES_DEBUG
	bkPrintf("LoadWholePackage: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

#if 0	// TP: testing
	if (LoadWholePackageBkg(name, NULL, group))
	{
		while (GetCurrentBkgLoadingStatus()==TRUE)
		{
			DrawLoadLoop(0);
		}
		
		return FindPackage(name);
	}
	else
	{
		bkPrintf("*** ERROR *** Could not load whole file\n");
		return NULL;
	}
#else
	TBPackageIndex  *pakIndex;
	PACKAGEENTRY	*package;

	if (pakIndex = FindPackage(name))
	{
		return pakIndex;	
	}

	pakIndex = LoadPackage(masterPakfile,name);

	if (pakIndex)
	{
		package = AddPackfileToIndex(pakIndex,name);
		package->state = PACKSTATE_RESIDENT;
		bkLoadPackageTextures(pakIndex);
		package->group = group;
		return pakIndex;
	}
	else
	{
		// PP: if anything went wrong, let's call it a bad disk
		if(pakIndex == NULL)
		{
			badDisk();
		}

		bkPrintf("*** ERROR *** Could not load whole file %s\n", name);
		return NULL;
	}
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadWholePackageBkg
	Purpose 	: Loads an entire package into memory
	Parameters 	: name
	Returns 	: true/false if the load was started succesfully
	Info 		: 
*/

bool LoadWholePackageBkg(char *name, char *actorToLoad, char group)
{
	TBPackageIndex *pakIndex;
	int				size;
	PACKAGEENTRY	*package;

#ifdef FILES_DEBUG
	bkPrintf("LoadWholePackageBkg: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

	// TP: Have a look and see if the package is already loaded
	if (FindPackage(name))
	{
		return true;
	}

	size = bkPackageFileLength(masterPakfile, name);

	if (size)
	{
		// TP: Add the packfile to the packlist ready for background loading
		package = AddPackfileToIndex(NULL,name);

		package->actorToLoad[0] = NULL;
		if (actorToLoad)
		{
			// TP: If the package should have an actor loaded from it automatically when loaded
			if (strlen(actorToLoad)<64)
			{
				strcpy(package->actorToLoad,actorToLoad);
			}
		}
		package->group = group;
		package->index = NULL;

		// TP: set the state to await loading, GetCurrentBkgLoadingStatus will kick off the actual load when ready
		package->state = PACKSTATE_AWAITINGLOAD;
		AddToLoadingBar(size);

		// PP: MUST RETURN A VALUE BECAUSE THE RETURN VALUE IS USED!!!!
		return true;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not load whole file %s\n", name);
		
		// PP: if anything went wrong, let's call it a bad disk
		badDisk();

		return false;
	}
}


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

void PackageLoaded(char *name, char *parms, void *data, void *context)
{
	PACKAGEENTRY	*ptr;
	uint32			crc;
	char			*message,*temp,*cp;
	TBBkgLoadResult	*loadResult = (TBBkgLoadResult*)data;

#ifdef FILES_DEBUG
	bkPrintf("PackageLoaded: \"%s\"\n", name);
#endif// PP: def FILES_DEBUG

	message = parms;
	temp = parms;

	while (temp)
	{
		temp = FindChar(temp,'\\');
		if (temp) temp++;
		if (temp) message = temp;
	}

	crc = bkCRC32((uchar *)message,strlen(message),0);

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->nameCRC == crc)
		{
			if (loadResult->resultCode==EBBKGERROR_NONE)
			{
				ptr->state = PACKSTATE_READING;
			}
			else
			{
				// TP: must kill package
				bkPrintf("Error loading package %s error code %d\n", ptr->name, loadResult->resultCode);
				RemovePackfileFromIndex(ptr->name);
				badDisk();
			}
			return;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: FreeWholePackage
	Purpose 	: Frees a whole package file and all resorces within it
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
void FreeWholePackageGroup(char group)
{
	PACKAGEENTRY	*ptr,*next;
#if BPLATFORM == PS2
	TBPackageID		id;
#endif

	for (ptr = packageList.head.next;ptr != &packageList.head;)
	{
		next = ptr->next;
		if (ptr->group == group)
		{
#if BPLATFORM == PS2
			id = bkGetPackageID(ptr->index);
			bkClosePackage(ptr->index);
			bkDeleteFilenameTable(id);
#else
			bkClosePackage(ptr->index);
#endif
			RemovePackfileFromIndex(ptr->name);
		}
		ptr = next;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeWholePackage
	Purpose 	: Frees a whole package file and all resorces within it
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
void FreeWholePackage(char *name)
{
	TBPackageIndex *pakIndex;
#if BPLATFORM == PS2
	TBPackageID		id;
#endif

	if (pakIndex = FindPackage(name))
	{
#if BPLATFORM == PS2
		id = bkGetPackageID(pakIndex);
		bkClosePackage(pakIndex);
		bkDeleteFilenameTable(id);
#else
		bkClosePackage(pakIndex);
#endif
		RemovePackfileFromIndex(name);
	}
	else
	{
		bkPrintf("*** ERROR *** Could not free whole file\n");
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FindPackage
	Purpose 	: Finds if a package is already loaded
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
TBPackageIndex	*FindPackage(char *name)
{
	PACKAGEENTRY	*ptr;
	uint32			crc;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->nameCRC == crc)
		{
			return ptr->index;
		}
	}
	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateIndex
	Purpose 	: Finds the package in memory and updates its pointer
	Parameters 	: name and new index
	Returns 	: 
	Info 		: This is needed on GameCube due to the way the bkFreePackageMemory 
				  changes the pointer to the package block
*/
void UpdateIndex(char *name, TBPackageIndex *newIndex)
{
	PACKAGEENTRY	*ptr;
	uint32			crc;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->nameCRC == crc)
		{
			ptr->index = newIndex;
			break;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: GetPackageState
	Purpose 	: Get the current state of a package
	Parameters 	: name
	Returns 	: 
	Info 		: 
*/
int	GetPackageState(char *name)
{
	PACKAGEENTRY	*ptr;
	uint32			crc;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->nameCRC == crc)
		{
			if (ptr->state == PACKSTATE_READING)
			{
				bkLoadPackageTextures(ptr->index);
				if (ptr->actorToLoad[0] != NULL)
				{
					LoadActor(ptr->actorToLoad,ptr->name,0);
				}
				ptr->state = PACKSTATE_RESIDENT;
			}
			return ptr->state;
		}
	}
	return -1;
}

/* --------------------------------------------------------------------------------
   Function : GetCurrentBkgLoadingStatus
   Purpose : Returns TRUE if there is any background loading still in progress and performs GetPackageState commands
   Parameters : 
   Returns : TRUE if bkg loading is still taking place
   Info : 
*/
int	GetCurrentBkgLoadingStatus(void)
{
	PACKAGEENTRY	*ptr, *awaitingLoad=NULL;
	int				retVal = FALSE;
	int				size;

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		if (ptr->state!=PACKSTATE_RESIDENT)
		{
			GetPackageState(ptr->name);

			if (ptr->state==PACKSTATE_AWAITINGLOAD)
			{
				awaitingLoad = ptr;
			}
			else
			{
				if (ptr->state!=PACKSTATE_RESIDENT)
				{
					retVal = TRUE;
				}
			}
		}
	}

	if (retVal==FALSE)
	{
		// TP: no loading currently taking place so if there is something awaiting loading then better start it now
		if (awaitingLoad)
		{
			if (awaitingLoad->index = LoadPackageBkg(masterPakfile, awaitingLoad->name, "load", &size))
			{
				if (awaitingLoad->state != PACKSTATE_READING)
				{
					awaitingLoad->state = PACKSTATE_LOADING;
				}
				retVal=TRUE;
			}			
			else
			{
				// TP: problem so remove entry from list
				RemovePackfileFromIndex(awaitingLoad->name);
			}
		}
	}

	return retVal;
}


/*  --------------------------------------------------------------------------------
	Function	: PrintPackageStates
	Purpose		: Prints the state of all packages in memory
	Parameters	: 
	Returns		: 
	Info		: 
*/

void PrintPackageStates(void)
{
	PACKAGEENTRY	*ptr;

	bkPrintf("Package status\n");
	bkPrintf("--------------\n");

	for (ptr = packageList.head.next;ptr != &packageList.head;ptr = ptr->next)
	{
		switch(ptr->state)
		{
		case PACKSTATE_NONE:
			bkPrintf("Package %s	- PACKSTATE_NONE\n", ptr->name);
			break;
		case PACKSTATE_AWAITINGLOAD:
			bkPrintf("Package %s	- PACKSTATE_AWAITINGLOAD\n", ptr->name);
			break;
		case PACKSTATE_LOADING:
			bkPrintf("Package %s	- PACKSTATE_LOADING\n", ptr->name);
			break;
		case PACKSTATE_READING:
			bkPrintf("Package %s	- PACKSTATE_READING\n", ptr->name);
			break;
		case PACKSTATE_RESIDENT:
			bkPrintf("Package %s	- PACKSTATE_RESIDENT\n", ptr->name);
			break;
		}
	}
}


/* --------------------------------------------------------------------------------
   Function		: openLoadingBooks
   Purpose		: set up and open the books to be displayed during loading
   Parameters	: 
   Returns		: 
   Info			: // PP: call this right before looping drawLoadLoop.
*/
void openLoadingBooks(void)
{
	setNumScreens(1);

	// PP: set up the level name textbox, ON EVERY LOOP  // PP: NOTE: I wouldn't have recommended this
	// TP: no level names during tournament, VIGZ's idea
	if ((levelNameStrings[map.sceneNumber]!=-1)/*&&(!gameStatus.multiplayer.Tournament)*/)
	{
		levelName->setText((EStringIndex)levelNameStrings[map.sceneNumber]);
		
		if (gameStatus.multiplayer.Tournament)
		{
			levelName->setFreeRect(RECTANGLE(1.0f, 1.0f, -0.5f, -0.35f));//-0.3
			levelName->setXAlign(PIFLAG_XALIGN_LEFT);

#ifndef CONSUMERDEMO
			loadingText->setYAlign(PIFLAG_YALIGN_CENTRE);
			loadingText->setFreeRect(RECTANGLE(TOURLOADINGBARX/2.0f + 0.005f, TOURLOADINGBARX/2.0f + 0.005f + LOADINGBARW, (TOURLOADINGBARY + LOADINGBARH)/2.0f - 0.005f, (TOURLOADINGBARY+LOADINGBARH)/2.0f  - 0.005f));
#endif

		}
		else
		{
			// PP: PLATFORM: ON XBOX, THIS TEXTBOX NEEDS TO BE LIFTED UP A WEE BIT INTO THE SAFE AREA.
			// PP: REMEMBER, THIS BOOK IS A NON-SAFE AREA BOOK SO THE SAFE-AREA AUTOMATION GOES OUT THE WINDOW
#if(BPLATFORM == XBOX)
			levelName->setFreeRect(RECTANGLE(-0.33f, 0.45f, -0.39f, -0.325f));// PP: bottom was -0.45f
#else// PP: !xbox
			levelName->setFreeRect(RECTANGLE(-0.33f, 0.45f, -0.45f, -0.325f));
#endif// PP: !xbox
			levelName->setXAlign(PIFLAG_XALIGN_LEFT);

#ifndef CONSUMERDEMO
			loadingText->setXAlign(PIFLAG_XALIGN_CENTRE);
			loadingText->setYAlign(PIFLAG_YALIGN_CENTRE);

			loadingText->setFreeRect(RECTANGLE(LOADINGBARX/2.0f + 0.005f, (LOADINGBARX)/2.0f + 0.005f + LOADINGBARW, (LOADINGBARY + LOADINGBARH)/2.0f - 0.005f, (LOADINGBARY+LOADINGBARH)/2.0f  - 0.005f));
#endif
		}

		levelBook->open();
	}
	else
	{
#ifndef CONSUMERDEMO
		levelName->setText((EStringIndex)levelNameStrings[SCENE_ZOOHUB]);// TP: doesn't matter what scene
		levelName->setFreeRect(RECTANGLE(1.0f, 1.0f, -0.5f, -0.35f));//-0.3
		levelName->setXAlign(PIFLAG_XALIGN_LEFT);
		loadingText->setXAlign(PIFLAG_XALIGN_CENTRE);
		loadingText->setYAlign(PIFLAG_YALIGN_CENTRE);

		loadingText->setFreeRect(RECTANGLE(LOADINGBARX/2.0f + 0.005f, (LOADINGBARX)/2.0f + 0.005f + LOADINGBARW, (LOADINGBARY + LOADINGBARH)/2.0f - 0.005f, (LOADINGBARY+LOADINGBARH)/2.0f  - 0.005f));
		levelBook->open();
#endif
	}
		
#ifdef CONSUMERDEMO

	// PP: go to the appropriate page on the Demo Objectives book

	if (map.sceneNumber == SCENE_DEPTSTR || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE)
	{
		if (gameStatus.multiplayer.numPlayers == 2)
			demoObjsBook->gotoPage("multi objectives");
		else
			demoObjsBook->gotoPage("single objectives");
		
		demoObjsBook->open();
	}

#endif
}


/* --------------------------------------------------------------------------------
   Function		: closeLoadingBooks
   Purpose		: close the books that were displayed during loading
   Parameters	: 
   Returns		: 
   Info			: // PP: call this right after looping drawLoadLoop.
*/
void closeLoadingBooks(void)
{
	// PP: close the Level Name book
	if (levelBook)
		levelBook->close(0.0f, TRUE);

#ifdef CONSUMERDEMO

	// PP: close the Demo Objectives book
	if (demoObjsBook)
		demoObjsBook->close();

#endif// PP: def CONSUMERDEMO

	BookList_update();
}


/* --------------------------------------------------------------------------------
   Function : DrawLoadLoop
   Purpose : main loop for loading
   Parameters : 
   Returns : 
   Info : 
*/

void DrawLoadLoop(int drawFlags)
{
	musicSystem.UpdateMusicSystem();

	// PP: update memory card display
	if (drawFlags&DRAW_MEMCARDUPDATE)
	{
		MemCard_update();
	}

	// PP: update the Book List (because we're in a seperate loop within the game loop)
	BookList_update();

	biReadDevices();

	if (drawFlags!=0)
	{
		// PP: override stereoscopy - it'd be more hassle than it's worth here, for the sake of that one little textbox...
		overrideStereoscopy(true);

		// prepare scene
		bdSetupView();
		bdBeginScene();

		applyFOV();
		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
		bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

		//draw fade
		if (drawFlags&DRAW_FADE)
		{
			camera[0].fade.DrawFade();
		}
		if (drawFlags&DRAW_SPLASH)
		{
			DrawSplashScreen(&gameStatus.splashScreen);
		}
		if (drawFlags&DRAW_LOADBAR)
		{
			DrawLoadingBar();
		}
		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		// PP: draw the Book List (because we're in a seperate loop within the game loop)
		BookList_draw();

		bdEndScene();

		// PP: finish timing the frame; calculate frames per second, frame length, etc.
		finishTimingFrame();

		if (controller1.f10DebounceChannel->value)
		{
			// take screen shot
			TakeScreenShot();
		}

	#ifdef _TAZDEBUG
		if (heapshow)
		{
			bkHeapShowGraphical(BALLGROUPS, 0.0f, standardFont);
	//			bkHeapShow();
		}
	#endif

		// PP: if the missing-pad flag is set, check for missing controllers and show message if one is missing
#if (BPLATFORM!=PC)
		checkForMissingControllers2((drawFlags&DRAW_MISSINGPAD) != 0);
#endif

		// PP: start timing the frame
		startTimingFrame();

		Flip(0, 0, 0, 128);

		// PP: finish overriding stereoscopy
		overrideStereoscopy(false);
	}
	else
	{
		// IH: New Babel, new danger. As from 63.00; bdFlip() needs to be called to update the background load...
		// IH: ...That is unless bUpdateBkgLoad() is called instead (TEMPORARY HACK)
	#if BPLATFORM==GAMECUBE
		bUpdateBkgLoad();
	#elif BPLATFORM==PS2
		// TP: WaitForVSync();
		bkSleep(20);
	#else
		bkUpdate();
	#endif
	}

	loadingBar.bytesLoaded = bBytesTransferred;
}

/*  --------------------------------------------------------------------------------
	Function	: PackageSize
	Purpose		: Return the size of a package
	Parameters	: pakIndex or NULL, name
	Returns		: 
	Info		: 
*/
extern TBFileIndex *bFindIndexFileByCRC(TBPackageIndex *index, uint32 crc);

int32 PackageSize(TBPackageIndex *pakIndex, char *name)
{
	TBFileIndex	*index;
	char	tempChar[256];

	if (pakIndex)
	{
#if		BPLATFORM==PS2
		sprintf(tempChar, "%s.ps2", name);
#elif	BPLATFORM==PC
		sprintf(tempChar, "%s.pc", name);
#elif	BPLATFORM==XBOX
		sprintf(tempChar, "%s.xbp", name);
#elif	BPLATFORM==GAMECUBE
		sprintf(tempChar, "%s.gcb", name);
#endif

		index = bFindIndexFileByCRC(pakIndex, bkCRC32((uchar*)tempChar,strlen(tempChar),0));
		if (index) return index->size;
		else
		{
			return 0;
		}
	}
	else
	{
		return bkPackageFileLength(name);
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialiseLoadingBar
   Purpose : Reset the loading bar values
   Parameters : 
   Returns : 
   Info : 
*/
void InitialiseLoadingBar(int offset)
{
	loadingBar.bytesLoaded = offset;
	loadingBar.totalBytesToLoad = offset;
	bBytesTransferred = offset;
}

/* --------------------------------------------------------------------------------
   Function : AddToLoadingBar
   Purpose : Increment the amount of bytes to be loaded
   Parameters : number of bytes to load
   Returns : 
   Info : 
*/
void AddToLoadingBar(int bytes)
{
	loadingBar.totalBytesToLoad += bytes;
}

/* --------------------------------------------------------------------------------
   Function : RemoveFromLoadingBar
   Purpose : Increment the amount of bytes that have been loaded
   Parameters : number of bytes loaded
   Returns : 
   Info : 
*/
void RemoveFromLoadingBar(int bytes)
{
	loadingBar.bytesLoaded += bytes;
}


/*  --------------------------------------------------------------------------------
	Function	: BytesOnLoadingBar
	Purpose		: Returns the number of bytes on the loading bar
	Parameters	: 
	Returns		: number of bytes
	Info		: 
*/

int BytesOnLoadingBar(void)
{
	return loadingBar.totalBytesToLoad;
}


/* --------------------------------------------------------------------------------
   Function : DrawLoadingBar
   Purpose : Draw the current state of the loading bar
   Parameters : 
   Returns : 
   Info : 
*/

//(-videoMode.yScreen/2.0f)+38.0f

void DrawLoadingBar(void)
{
	int i;
	TBPrimVertex	vert[4];
	float	tempFloat;
	float	x,y,w,h;
	float	colour;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,FALSE,FALSE);
	bdSetTexture(0, NULL);

#ifndef CONSUMERDEMO
	if (gameStatus.multiplayer.Tournament)
	{
		x = (float) (TOURLOADINGBARX * videoMode.xScreen/2.0f) - 2.0f;
		y = (float) (TOURLOADINGBARY * videoMode.yScreen/2.0f) - 2.0f;
	}
	else
	{
#endif
		x = (float) (LOADINGBARX * videoMode.xScreen/2.0f) - 2.0f;
		y = (float) (LOADINGBARY * videoMode.yScreen/2.0f) - 2.0f;
#ifndef CONSUMERDEMO
	}
#endif
	
	w = (float) (LOADINGBARW * videoMode.xScreen);
	h = (float) (LOADINGBARH * videoMode.yScreen);
	
	BDVERTEX_SETXYZW(&vert[0],x,y,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[1],x,y+h,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[3],x+w,y+h,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[2],x+w,y,0.0f,1.0f);

	for (i=0;i<4;i++) BDVERTEX_SETRGBA(&vert[i],0,0,0,255);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);

	if(loadingBar.totalBytesToLoad == 0)
	{
		tempFloat=0.0f;
	}
	else
	{
		tempFloat = (float)loadingBar.bytesLoaded / (float)loadingBar.totalBytesToLoad;
		if (tempFloat > 1.0f) tempFloat = 1.0f;
	}

	colour = tempFloat*128.0f;
	tempFloat *= (float) ((LOADINGBARW - 0.00625f) * videoMode.xScreen);

#ifndef CONSUMERDEMO
	if (gameStatus.multiplayer.Tournament)
	{
		x = (float) (TOURLOADINGBARX * videoMode.xScreen/2.0f);
		y = (float) (TOURLOADINGBARY * videoMode.yScreen/2.0f);
	}
	else
	{
#endif
		x = (float) (LOADINGBARX * videoMode.xScreen/2.0f);
		y = (float) (LOADINGBARY * videoMode.yScreen/2.0f);
#ifndef CONSUMERDEMO
	}
#endif

	w = (float)tempFloat;
	h = (float) ((LOADINGBARH - 0.00892857f) * videoMode.yScreen);	// JW: alright, it's a stupidly long number but you want to be exact dontcha?
	BDVERTEX_SETXYZW(&vert[0],x,y,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[1],x,y+h,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[3],x+w,y+h,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[2],x+w,y,0.0f,1.0f);

	for (i=0;i<4;i++) BDVERTEX_SETRGBA(&vert[i], i>1?(int)colour:0, i>1?(int)colour:0, 255, 255);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,TRUE,FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : ProcessConfigFile
   Purpose : Loads the config file into a structure
   Parameters : 
   Returns : 
   Info : 
*/
void ProcessConfigFile(void)
{
	#if BPLATFORM == PC
//		int fileSize = sizeof(tazData);

//		// Load PC config file
//		if ((tazData = (TAZDATA*)bkLoadFile(NULL, "taz.dat", NULL, &fileSize, NULL, 0))!=NULL)
//			tazDataLoaded = TRUE;


	HKEY	hKey;
    DWORD	dwBufLen = 80;
	char	datFile[80];

	memset(szProductLoc, 0, 80);
	
	if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY, 0, KEY_READ, &hKey))==ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, "Location", NULL, NULL, (BYTE *) szProductLoc, &dwBufLen)==ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
		}
	}
	
	sprintf(datFile, "%s\\taz.dat", szProductLoc);

	HANDLE hFile;	
	DWORD bytesRead;
	DWORD errorCode;

    // Create a file to store taz game setup data
	hFile = CreateFile(datFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	errorCode = GetLastError();

	// NH: Ok, this is a bit odd but blame Microsoft, and I'll let them explain:
	//     If the specified file exists before the function call and dwCreationDisposition is 
	//     CREATE_ALWAYS or OPEN_ALWAYS, a call to GetLastError returns ERROR_ALREADY_EXISTS,
	//     even though the function has succeeded. 
	if ((errorCode == 0) || (hFile != INVALID_HANDLE_VALUE))
	{
		// Read DataFile 
		ReadFile(hFile, &tazData, sizeof(TAZDATA), &bytesRead, NULL);
		
		// Close file
 		CloseHandle(hFile);

		if (tazData.version == DATA_FILE_VERSION)
		{
			tazDataLoaded = TRUE;			
		}
		else
		{
			bkPrintf("Incorrect 'Taz.dat' config file! Expected version");// = %f, loaded version = %f", DATA_FILE_VERSION, tazData.version); 
		}
	}
	#endif
}

#if BPLATFORM==PC
/*  --------------------------------------------------------------------------------
	Function	: LoadTazDataPC
	Purpose		: Use the taz config to setup to controllers etc
	Parameters	: 
	Returns		: 
	Info		: 
*/

void LoadTazDataPC()
{
	if (tazDataLoaded)
	{
		// NH: If controller 1 GUID exists set up the joystick 1
		if (memcmp(&tazData.player1.controllerGUID, &nullGuid, sizeof(GUID)) != 0)
			bJoystickInstanceGuid[0] = &tazData.player1.controllerGUID;

		// NH: If controller 2 GUID exists set up the joystick 2
		if (memcmp(&tazData.player2.controllerGUID, &nullGuid, sizeof(GUID)) != 0)
			bJoystickInstanceGuid[1] = &tazData.player2.controllerGUID;

		// NH: If Display Driver GUID exists set up the Display Adapter
		if (memcmp(&tazData.graphicsDetails.deviceIdentifier, &nullGuid, sizeof(GUID)) != 0)
		{
			bDisplayAdapterGuid = &tazData.graphicsDetails.deviceIdentifier;

			// NH: Set Draw Distance
			videoMode.farPlane = METERTOUNIT(tazData.graphicsDetails.drawDist);
			
			// NH: Set screen Mode
			if (!tazData.graphicsDetails.windowed)
			{
				videoMode.xScreen = tazData.graphicsDetails.width;
				videoMode.yScreen = tazData.graphicsDetails.height;
				videoMode.zScreen = tazData.graphicsDetails.zDepth;
				videoMode.bppScreen = tazData.graphicsDetails.colours;
				videoMode.flags &= ~BDISPLAYFLAG_WINDOW;
				videoMode.flagRequest = videoMode.flags;
			}
			else
			{
				videoMode.flags |= BDISPLAYFLAG_WINDOW;
				videoMode.flagRequest = videoMode.flags;
			}
		}

		// NH: If Sound Driver GUID exists set up the Sound Adapter
		if (memcmp(&tazData.soundDetails.soundDriverGUID, &nullGuid, sizeof(GUID)) != 0)
			bSoundHardwareGuid = &tazData.soundDetails.soundDriverGUID;

		// NH: Set Cartoon Lighting
		if (tazData.graphicsDetails.cartoonLighting)
		{
			// NH: Set cartoon lighting on
			gameStatus.customOptions &= ~CUSTOMOPTION_DISABLECARTOONLIGHTING;
		}
		else
		{
			// NH: Set cartoon lighting off
			gameStatus.customOptions |= CUSTOMOPTION_DISABLECARTOONLIGHTING;
		}	
		
		// NH: Set Cartoon Outlines
		if (tazData.graphicsDetails.cartoonOutline)
		{
			// NH: Set cartoon outline on
			gameStatus.customOptions &= ~CUSTOMOPTION_DISABLEOUTLINES;
		}
		else
		{
			// NH: Set cartoon outline off
			gameStatus.customOptions |= CUSTOMOPTION_DISABLEOUTLINES;
		}	

		// NH: Set Voodoo Compat mode
		if (tazData.graphicsDetails.doYouDoVoodoo)
		{
			// NH: Set Voodoo Compat mode on
			videoMode.flags |= BDISPLAYFLAG_NOTEXTURESTATES;
			videoMode.flagRequest = videoMode.flags;
		}
		else
		{
			// NH: Set Voodoo Compat mode off
			videoMode.flags &= ~BDISPLAYFLAG_NOTEXTURESTATES;
			videoMode.flagRequest = videoMode.flags;
		}	

		// NH: Set Language
		switch (tazData.language)
		{
		// NH: English
		case 0:
			GAMESTATUS::currentLanguage = BLANGUAGEID_UK;
			break;
		// NH: French
		case 1:
			GAMESTATUS::currentLanguage = BLANGUAGEID_F;
			break;
		// NH: German
		case 2:
			GAMESTATUS::currentLanguage = BLANGUAGEID_D;
			break;
		// NH: Italian
		case 3:
			GAMESTATUS::currentLanguage = BLANGUAGEID_IT;
			break;
		// NH: Spanish
		case 4:
			GAMESTATUS::currentLanguage = BLANGUAGEID_E;
			break;
		}
	}
	else
	{
		GAMESTATUS::currentLanguage = BLANGUAGEID_UK;
		
		// NH: Set cartoon lighting off (Default)
		gameStatus.customOptions |= CUSTOMOPTION_DISABLECARTOONLIGHTING;
	}
}
#endif // NH: #if BPLATFORM==PC

#if BPLATFORM==PC


/*  --------------------------------------------------------------------------------
	Function	: InitMissingCdWarning
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void InitMissingCdWarning(void)
{
	BOOK*			book;
	PAGE*			page;
	TEXTBOX*		textbox;

	book= &missingCdBook;

	book->setName("MissingCDBook\n");

	// PP: Add the page...

	page=book->addPage();

	page->insertItem(textbox = new TEXTBOX(STR_PC_NO_CD_TITLE));
	textbox->setStyle(TS_title)
		->setWrap(true)
		->setScale(0.75f);
	page->insertItem(textbox = new TEXTBOX(STR_PC_NO_CD_MESSAGE1),true);
	textbox->setWrap(true)
		->setScale(0.75f);
	page->insertItem(textbox = new TEXTBOX(STR_PC_NO_CD_INFO),true);
	textbox->setStyle(TS_info)
		->setWrap(true)
		->setScale(0.75f);

	book->setBackgroundColour(COLOUR(155,0,0,100));

	book->setEffectChooser(&basePIEChooser);
	book->flags &= ~BKFLAG_BOOKLIST;
}


/*  --------------------------------------------------------------------------------
	Function	: CheckCD
	Purpose		: Checks the CD and displays/handles and warnings
	Parameters	: 
	Returns		: 
	Info		: 
*/

void CheckCD()
{
	if (!ValidCD())
	{
		missingCdBook.flags &= ~BKFLAG_BOOKLIST; // NH: // PP: bodge
		missingCdBook.open(NULL, true);
		missingCdBook.privateUpdate();

		SETPROJECTION(2D);
		ENABLEBLEND(TRUE);
		SETBLEND(BLEND);
		SETZWRITE(FALSE);
		SETZTEST(ALWAYS);

		bdSetIdentityObjectMatrix();

		 // NH: ????
		bsPauseAllChannels(BSPAUSE);

		missingCdBook.privateDraw();

		Flip(0, 0, 0, 128);

		while ((!ValidCD()) && (!controller1.escDebounceChannel->value))
		{
			// PP: update sound
			bsUpdate((int)(gameStatus.deltaTime));

			// NH: Read controller State
			biReadDevices();

			bkSleep(0);
		}

		bsPauseAllChannels(BSUNPAUSE);

		// PP: start timing the frame
		startTimingFrame();

		// NH: No CD found, so quit
		if (gameStatus.noCDFound)
			gameStatus.quitNow = true;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ValidCD
	Purpose		: On the PC this checks that the game CD is in one of the drives
	Parameters	: 
	Returns		: true/false (CD in drive?)
	Info		: 
*/
bool ValidCD()
{
	#define VOL_NAME_LENGTH 20
	#define SYS_NAME_LENGTH 20
	#define DRV_STING_LEN 255

#ifdef _TAZDEBUG 
	return true;
#endif

#ifdef CONSUMERDEMO
	return true;
#endif

#ifndef INFOGRAMES
	return true;
#endif

	bool cdInDrive = false;

	char volumeName[VOL_NAME_LENGTH];
	char systemName[SYS_NAME_LENGTH];
	
	char driveStringBuffer[DRV_STING_LEN];
	
	DWORD driveStringBufferLength = DRV_STING_LEN;
	DWORD driveStringLength, maximumComponentLength, fileSystemFlags;

	char drive[4];
	int driveType;

	int driveStringPos = 0;

	// NH: Set error mode to Critical so that only critical errors are reported (and not no CD in drive)
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// NH: Get a string full of all the logical drive names
	driveStringLength = GetLogicalDriveStrings(DRV_STING_LEN, driveStringBuffer);

	// NH: While we are not at the end of the drive string list...
	while (driveStringPos < driveStringLength)
	{
		// NH: Copy the drive (starting at the 'driveStringPos') into 'drive'
		strcpy(drive, &driveStringBuffer[driveStringPos]);
		// NH: Increment the 'driveStringPos' so that it points to the start of the next Drive
		driveStringPos += strlen(drive) + 1;
		
		// NH: Check to see if the drive is a CD-ROM
		if (GetDriveType(drive) == DRIVE_CDROM)
		{
			// NH: Get info on the supplied drive
			if (GetVolumeInformation(drive, volumeName, VOL_NAME_LENGTH, NULL, &maximumComponentLength, &fileSystemFlags, systemName, SYS_NAME_LENGTH))
			{
				// NH: Check that the name matches what we are expecting, if so set the 'cdInDrive' flag to true
				if(strcmp(volumeName, CD_VOLUME_NAME) == 0)
				{
					cdInDrive = true;
				}
			}
		}
	}

	// NH: Set error mode back to the system default
	SetErrorMode(0);
	
	if (!cdInDrive)
	{
		// NH: CD not in drive, Set flags so that the program display the noCD message
		gameStatus.noCDFound	= true;
	}
	else
	{
		bkPrintf("CD Found\n");
		gameStatus.noCDFound	= false;
	}

	return cdInDrive;
}

/*  --------------------------------------------------------------------------------
	Function	: CheckForOtherTazInstances
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

bool CheckForOtherTazInstances()
{
	bool bFound;
	// NH: Prevent more than 1 instance of Taz and stop launcher re-opening if game running
	HANDLE hMutexOneInstance;
	hMutexOneInstance = CreateMutex(NULL,TRUE,"PreventTazWantedGameInstances");
    
	if(GetLastError() == ERROR_ALREADY_EXISTS)
        bFound = TRUE;
    if(hMutexOneInstance) 
        ReleaseMutex(hMutexOneInstance);

	if (bFound == TRUE)
	{
		bkPrintf("%Found another Instance of Taz:Wanted running on this machine. Can't open a new version");
		return true;
	}
	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: DrawMouse
	Purpose		: like it says
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawMouse()
{
	if (mouse.active)
	{
		bdBeginScene();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
		bdSetObjectMatrix(bIdentityMatrix);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		bdSetTexture(0, NULL);
		
		// NH: Update pointer
		UpdateMousePointer(&mouse);
		// NH: Draw pointer
		DrawMousePointer(&mouse);

		bdEndScene();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}
}
#endif // NH: #if BPLATFORM==PC


/*	--------------------------------------------------------------------------------
	Function 	: badDisk
	Purpose 	: handle a fatal disk read error
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this has to interrupt whatever loading was going on at the time
					// PP: I kept the name short because I can see us using this function a lot.
*/
void badDisk(void)
{
	BOOK						book;
	PAGE*						page;
//	PAGE*						subPage;

	// PP: standard colour used as a background for messages such as bad disk and missing pad, to subdue whatever is on screen at the time (might even be other text)
	extern COLOUR				standardMessageBackgroundTint;// PP: pause.cpp

	// PP: font size for the bad-disk message
#define BADDISK_FONTSIZE	0.8f	

	// PP: ===============
	// PP: SET UP THE BOOK
	// PP: (SINCE YOU'LL ONLY EVER SEE THIS BOOK ONCE IN ANY GAME SESSION,
	// PP: THERE'S NO POINT PREPARING IT IN ADVANCE!
	// PP: ===============

	book.setBookRect(RECTANGLE(-0.35f, 0.35f, -0.35f, 0.35f));

	page=book.addPage();

	page->insertItem(STR_XBOX_TCR_BAD_GAME_DISK)
		->setFontSize(BADDISK_FONTSIZE)
		->setWrap(true);

	// PP: these two strings mention Xbox; you'll need different versions for other platforms
#if(BPLATFORM == XBOX)

	// PP: "but don't worry, your game is saved"
	// TP: America don't like this message
	if (videoMode.flags&BDISPLAYFLAG_PAL)
	{
		page->insertItem(STR_XBOX_GAME_SAVED_DONT_PANIC)
			->setFontSize(BADDISK_FONTSIZE)
			->setWrap(true);
	}

	// PP: "now go to the dashboard and weep - WEEP LIKE A LITTLE CHILD - AHAHAHAHAAAAAH!!"
/*
TP:	Not allowed to, TCR 6-09

	page->insertItem(STR_XBOX_TCR_A_DASHBOARD)
		->setFontSize(BADDISK_FONTSIZE)
		->setStyle(TS_control)
		->setWrap(true);
*/

#endif// PP: xbox

	// PP: set a full-screen background tint
	book.setFullScreenBackground(standardMessageBackgroundTint);

	book.open(NULL, true);
	book.privateUpdate();

	SETPROJECTION(2D);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);

	// PP: NEW (30.4.02): put all (r,g,b) framewrite on for this message (might be in stereoscopic mode)
	SETFRAMEWRITE(BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE);

	bdSetIdentityObjectMatrix();

	bsPauseAllChannels(BSPAUSE);

	// PP: NEW: pause vibrations on controller1 and controller2
	// PP: NOTE: must continually poll bkUpdate for the vibrations to actually stop
	PauseRumble(&controller1);
	PauseRumble(&controller2);

	book.privateDraw();

#ifdef CONSOLEDEBUG
		DrawConsole();
#endif
	Flip(0, 0, 0, 128);

	while(true)
	{
		biReadDevices();

		// PP: update sound
		bsUpdate((int)(gameStatus.deltaTime));

#if(BPLATFORM == XBOX)

#if 0	// TP: not allowed to do this TCR 6-09

		// PP: ===================
		// PP: HANDLE PLAYER INPUT
		// PP: ===================

		// PP: 'A' takes the player to the dashboard, where they can weep like a little child.
		// PP: "YOUR GAME IS BROKEN, LITTLE CHILD!  DO YOU SEE HOW THAT WORKS?!  HAAAHAHAHAHAAAA!!"
		if(OKBUTTON_PADNUM(1)
			||OKBUTTON_PADNUM(2)
#if(NUM_CONTROLLER_PORTS == 4)
			||OKBUTTON_PADNUM(3)
			||OKBUTTON_PADNUM(4)
#endif// PP: 4 ports
			)
		{
			LD_LAUNCH_DASHBOARD		dashLaunchData;

			dashLaunchData.dwReason=XLD_LAUNCH_DASHBOARD_MAIN_MENU;	// PP: go to the main dashboard menu, why not
			dashLaunchData.dwContext=0;								// PP: A title-defined value that will be returned to the title in the LD_FROM_DASHBOARD structure (see XGetLaunchInfo) when the Dashboard reboots to the title after performing whatever service the title has asked of the Dashboard.
			dashLaunchData.dwParameter1=0;							// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 
			dashLaunchData.dwParameter2=0;							// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 

			XLaunchNewImage(NULL,// PP: dashboard
							(PLAUNCH_DATA)&dashLaunchData);
		}
#endif
		
#endif// PP: xbox

		// PP: bkUpdate is the guts of bdFlip
		// PP: needed to keep streaming music ticking over, also to update the vibration (allowing it to stop)
		bkUpdate();

		bkSleep(0);
	}
}