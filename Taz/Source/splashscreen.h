#ifndef __H_SPLASHSCREEN
#define __H_SPLASHSCREEN

enum ESplashScreenStorage
{
	SPLASHSCREENSTORAGE_MEMORY,
	SPLASHSCREENSTORAGE_LOADED,
};

typedef struct TAG_SPLASHSCREEN
{
	TBSplash				*resource;
	ESplashScreenStorage	storage;
	char					name[256];
} SPLASHSCREEN BALIGN16;

/* --------------------------------------------------------------------------------
   Function : LoadSplashScreen
   Purpose : Load splash screen and fill in SPLASHSCREEN structure
   Parameters : SPLASHSCREEN, file name, pakname
   Returns : 
   Info : 
*/
int LoadSplashScreen(SPLASHSCREEN *splashScreen, char *name, char *pakName);

/* --------------------------------------------------------------------------------
   Function : DrawSplashScreen
   Purpose : Draw splash screen
   Parameters : SPLASHSCREEN
   Returns : 
   Info : 
*/
void DrawSplashScreen(SPLASHSCREEN *splashScreen);

/* --------------------------------------------------------------------------------
   Function : FreeSplashScreen
   Purpose : Remove the splash screen from memory
   Parameters : SPLASHSCREEN
   Returns : 
   Info : 
*/
void FreeSplashScreen(SPLASHSCREEN *splashScreen);

/*  --------------------------------------------------------------------------------
	Function	: LoadCorrectSplashScreen
	Purpose		: Checks if the correct splash screen is loaded, if not it loads it.
	Parameters	: scene
	Returns		: 
	Info		: 
*/
void LoadCorrectSplashScreen(SPLASHSCREEN *splashScreen, int scene);


#endif
