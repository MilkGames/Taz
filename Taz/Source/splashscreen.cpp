// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : splashscreen.cpp
//   Purpose : functions to load draw and free splash screens
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "splashscreen.h"
#include "files.h"
#include "scene.h"
#include "tournament.h"
#include "main.h"
#include "display.h"

#if BPLATFORM == PS2
//#define _USE_BACKGROUND_LOAD_PS2_
#endif

// ** ASL 27/03/2002
#ifdef _USE_BACKGROUND_LOAD_PS2_

// template function to async load splash screens
TBSplash *asyncSplashLoad(char *splashName);

// splash screen package filename. externed from file.cpp
extern char splashFilename[260];


/* --------------------------------------------------------------------------------
   Function : asyncEventCallback
   Purpose : splash screen background load callback
   Parameters : event name, event parms, context [int loaded]
   Returns : 
   Info : 
*/

void asyncEventCallback(char *name, char *parms, void *data, void *context)
{
	char	*errstr;
	int		errnum;


	// default: no error
	errnum = 0;

	// possible read error occurred?
	errstr = strstr(parms, BSTREAMFAIL_EVENTPARMSTRING);
	if (errstr)
	{
		errstr += strlen(BSTREAMFAIL_EVENTPARMSTRING);
		errnum = atoi(errstr);
	}

	((int *)context)[1] = errnum;
	((int *)context)[0] = 1;

	if (errnum)
		bkPrintf("asyncEventCallback called: load failed, parms: %s\n", parms);
	else
		bkPrintf("asyncEventCallback called: loaded, parms: %s\n", parms);
}


/* --------------------------------------------------------------------------------
   Function : asyncSplashLoad
   Purpose : async load splash screen
   Parameters : splash screen name
   Returns : splash resource pointer or NULL error
   Info : 
*/

TBSplash *asyncSplashLoad(char *splashName)
{
	uint			crc;
	TBEventClient	*eventClient;
	TBResourceInfo	*res;
	TBSplash		*splash;
	int				size;
	volatile int	loadInfo[2];


	// need to make sure that the splash pakfile is open
	BASSERT(BASSERT_GENERAL, (splashPakfile != NULL), "asyncSplashLoad : Splash pakfile needs to be open\n");

	// fix the splash screen CRC. screen type is appended
	crc = bdFixSplashCRC(bkStringCRC(splashName));


	// ** Background load

	// create load event and client
	if (bkCreateEvent("_ASYNC_SPLASH_LOAD_EVENT_") != OK)
		return NULL;
	eventClient = bkTrapEventCallback("_ASYNC_SPLASH_LOAD_EVENT_", asyncEventCallback, (void *)&loadInfo);

	// load splash screen
	loadInfo[0] = loadInfo[1] = 0;
	res = (TBResourceInfo *)bkLoadFileByCRCBkg(splashPakfile, splashFilename, NULL, &size, NULL, "_ASYNC_SPLASH_LOAD_EVENT_", crc);
	if (res != NULL)
	{
		// wait for splash screen load to complete or fail
		while (!loadInfo[0])
		{
			bkSleep(0);
		}

		// did load fail?
		switch (loadInfo[1])
		{
		case EBSTREAMFAIL_NOFAIL:
		case EBSTREAMFAIL_EMPTY_PACKAGE:
			break;
		default:
			FREE((uchar *)res);
			res = NULL;
			break;
		}
	}

	// remove event handler
	bkDeleteEventClient(eventClient);
	bkDeleteEvent("_ASYNC_SPLASH_LOAD_EVENT_");

	// file not loaded.. not found?
	if (res == NULL)
		return NULL;


	// get splash screen from loaded resource
	splash = (TBSplash *)res;

	// fix-up the pointer to the pixels
	splash->data[0] = splash->data[1] = (uchar*)(splash+1);

  	// set the resource CRC and add to pool
	res->crc = crc;
	bAddGlobalResource(res, splashPakfile, EBRESTYPE_SPLASH, BDEFAULTGROUP);

	return splash;
}


/* --------------------------------------------------------------------------------
   Function : LoadSplashScreen
   Purpose : background Load splash screen and fill in SPLASHSCREEN structure
   Parameters : SPLASHSCREEN pointer, splash file name, pakname
   Returns : 
   Info : assumes that splashPakfile contains the opened splash package index
*/

int LoadSplashScreen(SPLASHSCREEN *splashScreen, char *name, char *pakName)
{
	static char	splashName[256];


	// if PAL mode we need to use PAL splash screens
	sprintf(splashName, "splash%s\\%s", videoMode.flags & BDISPLAYFLAG_NTSC ? "" : "pal", name);

	// lets see if the splash screen is already loaded
	splashScreen->resource = (TBSplash *)bkLoadResource(NULL, splashName, EBRESTYPE_SPLASH, BDEFAULTGROUP);
	if (splashScreen->resource) 
	{
		// flag as resident in memory
		splashScreen->storage = SPLASHSCREENSTORAGE_MEMORY;
		return TRUE;
	}

	// background load splash screen and wait
	splashScreen->resource = asyncSplashLoad(splashName);

	// any luck?
	if (splashScreen->resource != NULL)
	{
		// flag as loaded
		splashScreen->storage = SPLASHSCREENSTORAGE_LOADED;
		return TRUE;
	}

	// splash screen not found
	return FALSE;
}

#else

/* --------------------------------------------------------------------------------
   Function : LoadSplashScreen
   Purpose : Load splash screen and fill in SPLASHSCREEN structure
   Parameters : SPLASHSCREEN, file name, pakname
   Returns : 
   Info : 
*/
int LoadSplashScreen(SPLASHSCREEN *splashScreen, char *name, char *pakName)
{
	TBPackageIndex	*pakIndex;
	char			tempChar[256];
	bool			iOpenedPack=false;
	int				i;

#if(BPLATFORM == PS2)
#ifdef CONSUMERDEMO
	#ifndef STANDALONE
		char	tempName[256];

		#ifndef USE_HOST0
			sprintf(tempName, "tazdemo\\%s", pakName);	

			pakName = tempName;
		#endif
	#endif
#endif
#endif// PP: not xbox

#if BPLATFORM==PS2
		if (videoMode.flags & BDISPLAYFLAG_NTSC)
		{
			sprintf(tempChar, "splash\\%s", name);
		}
		else
		{
			sprintf(tempChar, "splashpal\\%s", name);
		}
#elif BPLATFORM==GAMECUBE
		if(videoMode.flags & BDISPLAYFLAG_NTSC)
			sprintf(tempChar, "splashgc\\%s", name);
		else
			sprintf(tempChar, "splashgc_pal\\%s", name);
#else
		sprintf(tempChar, "splashpc\\%s", name);
#endif

	bkPrintf("LoadSplashScreen(%s,%s)\n",tempChar,pakName);

	strcpy(splashScreen->name, name);

	// TP: Test load from memory first.
	splashScreen->resource = (TBSplash *)bkLoadResource(NULL, tempChar, EBRESTYPE_SPLASH, BDEFAULTGROUP);
	if (splashScreen->resource) 
	{
		splashScreen->storage = SPLASHSCREENSTORAGE_MEMORY;
		return TRUE;
	}

	// TP: If not loaded from memory
	// TP: attempt 3 retries
	// TP: for (i=0;i<3;i++)
	{
		pakIndex = FindPackage(pakName);

		if (!pakIndex)
		{
			if (splashPakfile)
			{
				pakIndex = splashPakfile;
			}
			else
			{
				pakIndex = bkOpenPackage(pakName);
				iOpenedPack = true;
			}
		}

		if (pakIndex)
		{
			splashScreen->resource = (TBSplash *)bkLoadResource(pakIndex,tempChar,EBRESTYPE_SPLASH,BDEFAULTGROUP);
			if (!splashScreen->resource)
			{
				// TP: try and load from old place for maps
				sprintf(tempChar,"extras\\%s",name);
				splashScreen->resource = (TBSplash *)bkLoadResource(pakIndex,tempChar,EBRESTYPE_SPLASH,BDEFAULTGROUP);
			}
			if (iOpenedPack) bkClosePackage(pakIndex);

			if (splashScreen->resource)
			{
				splashScreen->storage = SPLASHSCREENSTORAGE_LOADED;
				return TRUE;
			}
		}
	}

	bkPrintf("*** ERROR *** Package %s not loaded\n",pakName);
	return FALSE;
}
#endif

/* --------------------------------------------------------------------------------
   Function : DrawSplashScreen
   Purpose : Draw splash screen
   Parameters : SPLASHSCREEN
   Returns : 
   Info : 
*/
void DrawSplashScreen(SPLASHSCREEN *splashScreen)
{
	if (splashScreen->resource)
	{
#if ((BPLATFORM == PC)||(BPLATFORM == XBOX))// PP: BSPLASHFLAG_SCALETOFIT works on Xbox too thank goodness
		bdDrawSplash(splashScreen->resource,-1,-1,BSPLASHFLAG_SCALETOFIT);
#else
		bdDrawSplash(splashScreen->resource,-1,-1);
#endif
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeSplashScreen
   Purpose : Remove the splash screen from memory
   Parameters : SPLASHSCREEN
   Returns : 
   Info : 
*/
void FreeSplashScreen(SPLASHSCREEN *splashScreen)
{
	if (splashScreen->resource)
	{
		// TP: only delete the resource if the splashscreen was created by this code
		if (splashScreen->storage==SPLASHSCREENSTORAGE_LOADED)
		{
			bkDeleteResource(splashScreen->resource);
		}
		splashScreen->resource = NULL;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: LoadCorrectSplashScreen
	Purpose		: Checks if the correct splash screen is loaded, if not it loads it.
	Parameters	: scene
	Returns		: 
	Info		: 
*/
void LoadCorrectSplashScreen(SPLASHSCREEN *splashScreen, int scene)
{
	char	*splash;
	static char	curScreen=0;
	char	tempChar[256];

#ifdef CONSUMERDEMO
	if (map.sceneNumber != SCENE_LANGUAGESELECT)
	{
		if (gameStatus.multiplayer.versusCPU)
			splash = "demo03.bmp";
		else
			splash = "demo04.bmp";
	}
	else
		splash = "demo00.bmp";
#else
	if ((map.sceneNumber == SCENE_LANGUAGESELECT)||(map.lastScene == SCENE_LANGUAGESELECT))
	{
		splash = "splash04.bmp";
	}
	else
	{
		if (gameStatus.multiplayer.Tournament)
		{
			splash = GetTournamentSplashScreen();
		}
		else
		{
			switch(curScreen)
			{
			case 0:
				splash = "splash.bmp";
				break;
			case 1:
				splash = "splash02.bmp";
				break;
			case 2:
				splash = "splash04.bmp";
				break;
			case 3:
				splash = "splash5.bmp";
				break;
			}
			
			curScreen++;
			if (curScreen>3) curScreen=0;
		}
	}
#endif
	
#if BPLATFORM==PS2
		if (videoMode.flags&BDISPLAYFLAG_NTSC)
		{
			sprintf(tempChar, "splash\\%s", splash);
		}
		else
		{
			sprintf(tempChar, "splashpal\\%s", splash);
		}
#elif BPLATFORM==GAMECUBE
		if(videoMode.flags & BDISPLAYFLAG_NTSC)
			sprintf(tempChar, "splashgc\\%s", splash);
		else
			sprintf(tempChar, "splashgc_pal\\%s", splash);
#else
		sprintf(tempChar, "splashpc\\%s", splash);
#endif
	
#if (BPLATFORM == PS2)
	if (!bkLoadResource(NULL,tempChar,EBRESTYPE_SPLASH,BDEFAULTGROUP))
#endif
		LoadSplashScreen(splashScreen,splash,"splshscr");
}
