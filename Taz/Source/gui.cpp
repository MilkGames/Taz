// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : gui.cpp
//   Purpose : functions to create and display the onscreen gui
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"

// TP: Same version number to be used across the board from now on, I will change it for each build
#if BPLATFORM == XBOX
	#define TAZ_VERSIONNUM	("Version MC4.3")
#else
	#define TAZ_VERSIONNUM	("Version MC4.3")
#endif

#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "display.h"
#include "main.h"
#include "scene.h"
#include "gui.h"
#include "objectviewer.h"
#include "maths.h"
#include "fade.h"
#include "timer.h"
#include "text2.h"
#include "genericai.h"
#include "zookeeper.h"
#include "bears.h"
#include "alligator.h"
#include "tazcatcher.h"
#include "securitybot.h"
#include "apes.h"
#include "museumguard.h"
#include "shark.h"
#include "status.h"
#include "files.h"
#include "phonebox.h"
#include "constructionbot.h"
#include "constructionworker.h"
#include "characters.h"
#include "animation.h"
#include "lights.h"
#include "invisibletaz.h"
#include "chillipepper.h"
#include "bubblegum.h"
#include "physics.h"
#include "envmap.h"
#include "music.h"
#include "taskforce.h"
#include "memcard.h"
#include "anvilgang.h"
#include "sfx.h"
#include "bjshellac.h"
#include "tribalchief.h"
#include "nastycan.h"
#include "doggy.h"
#include "playerstats.h"
#include "minerbot.h"
#include "zooboss.h"
#include "destruct.h"
#include "race.h"

#include "cityboss.h"
#include "westboss.h"
#include "tazboss1.h"
#include "mtweetyscripts.h"
#include "font.h"					// PP: for LoadStringTable
#include "collectibles.h"

#ifdef INFOGRAMES
#define NOAUTOLOADENEMIES
#else
//#define NOAUTOLOADENEMIES
#endif
//#define MARKETING

// PP: maximum allowable length of a string in the gui
#define GUI_MAX_STRING_LENGTH		30

TBGuiElement	*mainMenu;
static TBGuiElement	*fileMenu;
static TBGuiElement	*viewMenu;
static TBGuiElement	*videoMenu;
static TBGuiElement	*sceneMenu;
static TBGuiElement	*optionMenu;
static TBGuiElement	*cheatMenu;
static TBGuiElement	*bountyMenu;
static TBGuiElement	*qaMenu;

static TBGuiElement	*characterMenu;
static TBGuiElement	*enemyMenu;
static TBGuiElement	*powerUpMenu;
static TBGuiElement	*optimisationMenu;

// TP: fog stuff
static TBGuiElement	*fogMenu;
static TBGuiElement	*fogMin;
static TBGuiElement	*fogMax;
static TBGuiElement	*fogR;
static TBGuiElement	*fogG;
static TBGuiElement	*fogB;

// TP: background stuff
static TBGuiElement *backMenu;
static TBGuiElement *backHeight;
static TBGuiElement *backAngle;


static TBGuiMenuItem	*fogToggle;
static TBGuiMenuItem	*burpCan;
static TBGuiMenuItem	*hiccupCan;
static TBGuiMenuItem	*chilliPepper;
static TBGuiMenuItem	*testTube;
static TBGuiMenuItem	*bubblegum;
static TBGuiMenuItem	*ingameView;
static TBGuiMenuItem	*objectView;
TBGuiMenuItem			*guiFlyCam=NULL;
static TBGuiMenuItem	*PAL;
static TBGuiMenuItem	*NTSC;
static TBGuiMenuItem	*multiplayerOption;
static TBGuiMenuItem	*cameraColl;
TBGuiMenuItem			*m16x9=NULL;
static TBGuiMenuItem	*guiDebugDPad;
static TBGuiMenuItem	*versusCPU;
static TBGuiMenuItem	*guiToggleSplitScreenMode;
static TBGuiMenuItem	*instanceName;
static TBGuiMenuItem	*debugDestruct;
static TBGuiMenuItem	*debugFood;
static TBGuiMenuItem	*instanceZone;
static TBGuiMenuItem	*recordDemo;
static TBGuiMenuItem	*playDemo;
static TBGuiMenuItem	*playHostDemo;


static TBGuiMenuItem	*showInvisible;
static TBGuiMenuItem	*showStrips;

// NH: Optimisation menu items
static TBGuiMenuItem	*cartoonLighting;
static TBGuiMenuItem	*outlines;
static TBGuiMenuItem	*incDrawDistance;
static TBGuiMenuItem	*decDrawDistance;
static TBGuiMenuItem	*graph;
static TBGuiMenuItem	*heap;

static TBGuiMenuItem	*music;
static TBGuiMenuItem	*console;

static TBGuiMenuItem	*addBOne;
static TBGuiMenuItem	*addBTen;
static TBGuiMenuItem	*addBHun;
static TBGuiMenuItem	*addBTho;
static TBGuiMenuItem	*addBTTh;
static TBGuiMenuItem	*addBHTh;

static float	guiTime=10.0f;	// TP: time since last gui input

enum 
{
	GUI_EXIT,
	GUI_LOADMAP,
	GUI_NTSC,
	GUI_PAL,
	GUI_OBJECTVIEWER,
	GUI_INGAMEVIEW,
	GUI_FLYCAM,
	GUI_MULTIPLAYER,
	GUI_VERSUSCPU,
	GUI_TOGGLESPLITSCREENMODE,
	GUI_16x9,
	GUI_CHRISCAM,
	GUI_SHOWINVISIBLE,
	GUI_OUTLINEONLY,
	GUI_NORMAL,
	GUI_CHARTAZ = 50,
	GUI_ENEMYTAZ = 100,
	GUI_SHOWSTRIPS = 150,
	GUI_NOCHEATS = 200,								// PP: GUI element index of the first cheat (I think)(?)
	GUI_LASTCHEAT = GUI_NOCHEATS+NUM_CHEATS+1,		// PP: GUI element index of the last cheat (I think)(?)
	GUI_TIMER,
	GUI_ZOOKEEPER,
	GUI_POLARBEAR,
	GUI_APE,
	GUI_PIRATEAPE,
	GUI_ALLIGATOR,
	GUI_SHARK,
	GUI_TAZCATCHER,
	GUI_BURPCAN,
	GUI_HICCUPCAN,
	GUI_CHILLIPEPPER,
	GUI_TESTTUBE,
	GUI_BUBBLEGUM,
	GUI_INSTANCENAME,
	GUI_INSTANCEZONE,
	GUI_DEBUGDESTRUCT,
	GUI_DEBUGFOOD,
	GUI_FOGMAXINC,
	GUI_FOGMAXDEC,
	GUI_FOGMININC,
	GUI_FOGMINDEC,
	GUI_FOGRINC,
	GUI_FOGRDEC,
	GUI_FOGGINC,
	GUI_FOGGDEC,
	GUI_FOGBINC,
	GUI_FOGBDEC,
	GUI_FOGTOGGLE,
	GUI_CARTOONLIGHTING,
	GUI_OUTLINES,
	GUI_INCDRAWDISTANCE,
	GUI_DECDRAWDISTANCE,
	GUI_BACKHEIGHTINC,
	GUI_BACKHEIGHTDEC,
	GUI_BACKROTLEFT,
	GUI_BACKROTRIGHT,
	GUI_GRAPH,				// LH: Added for checking performance
	GUI_HEAPSHOW,			// LH: Added for checking memory
	GUI_MUSIC,
	GUI_CONSOLE,
	GUI_RECORDDEMO,
	GUI_PLAYDEMO,
	GUI_PLAYHOSTDEMO,
	GUI_DEBUGDPAD,
	GUI_ADDBOUNTYONE,
	GUI_ADDBOUNTYTEN,
	GUI_ADDBOUNTYHUNDRED,
	GUI_ADDBOUNTYTHOUSAND,
	GUI_ADDBOUNTYTENTHOU,
	GUI_ADDBOUNTYHUNTHOU,
	GUI_FINISHLEVEL,
	GUI_COMPLETEALLPOSTERS,
	GUI_FORCEWIN,
	GUI_FORCELOSS,

	// must be last entry
	GUI_SCENESELECT,
};

TBGuiElement	*zooLevelMenu;
char *zooLevels[] = {
	"Zoo - Hub",
	"Zoo - Ice Kingdome",
	"Zoo - Safari",
	"Zoo - Aqua",
	"Zoo - Boss",
	NULL
};

TBGuiElement	*cityLevelMenu;
char *cityLevels[] = {
	"City - Hub",
	"City - Dept Store",
	"City - Museum",
	"City - Construct",
	"City - Boss",
	NULL
};

TBGuiElement	*westLevelMenu;
char *westLevels[] = {
	"West - Hub",
	"West - Ghost Town",
	"West - Gold Mine",
	"West - Grand Canyon",
	"West - Boss",
	NULL
};

TBGuiElement	*tazLevelMenu;
char *tazLevels[] = {
	"Taz - Tazmania",
	"Taz - Boss 1",
	"Taz - Boss 2",
	NULL
};

TBGuiElement	*miscLevelMenu;
char *miscLevels[] = {
	"Language Select",
	"Front End",
	"Enemy Playpen",
	NULL
};

TBGuiElement	*vrLevelMenu;
char *vrLevels[] = {
	"Vehicle Race - Museum",
	"Vehicle Race - Grand Canyon",
	"Vehicle Race - Dept Store",
	NULL
};

TBGuiElement	*deLevelMenu;
char *deLevels[] = {
	"Destruction - Aqua",
	"Destruction - Construct",
	"Destruction - Tazmania",
	NULL
};

TBGuiElement	*taLevelMenu;
char *taLevels[] = {
	"Time Attack - Ice Dome",
	"Time Attack - Gold Mine",
	"Time Attack - Safari",
	NULL
};

TBGuiElement	*zooEnemyMenu;
char *zooEnemies[] = {
	"polarbear.obe",
	"keeper.obe",
	"catcher.obe",
	"alligator.obe",
	"browbear.obe",
	"polarbear.obe",
	"pirateape.obe",
	"shark.obe",
	NULL
};

TBGuiElement	*cityEnemyMenu;
char *cityEnemies[] = {
	"security bot.obe",
	"guard.obe",
	"taskforce.obe",
	"doggy.obe",
	"catcher.obe",
	"anvilgang.obe",
	"construction worker.obe",
	"construction bot.obe",
	NULL
};

TBGuiElement	*westEnemyMenu;
char *westEnemies[] = {
	"beaky.obe",
	"bjshellac.obe",
	"demondog.obe",
	"catcher.obe",
	"minerbot.obe",
	"anvilgang.obe",
	"nastycan.obe",
	NULL
};

TBGuiElement	*tazEnemyMenu;
char *tazEnemies[] = {
	"tribalchief.obe",
	"anvilgang.obe",
	"parrot.obe",
	"mechatweety.obe",
	NULL
};

char	*characterNames[] = {
	"tazdevil.obe",
	"polarbear.obe",
	"safarihat.obe",
	"coyote.obe",
	"spindevil.obe",
	"surfdevil.obe",
	"safaridevil.obe",
	"daft.obe",
	"gossamer.obe",
	"keeper.obe",
	"sylvester.obe",
	"tweety.obe",
	"tycoon.obe",
	"yosemite.obe",
	"alligator.obe",
	"shark.obe",
	"elephant.obe",
	"mrfudd.obe",
	"catcher.obe",
	"ape.obe",
	"irrepresible.obe",
	"browbear.obe",
	"pirateape.obe",
	"security bot.obe",
	"guard.obe",
	"phonebox.obe",
	"construction bot.obe",
	"construction worker.obe",
	"beaky.obe",
	"bjshellac.obe",
	"demondog.obe",
	"construction worker.obe",
	"mechatweety.obe",
	"doggy.obe",
	"parrot.obe",
	"nastycan.obe",
	"anvilgang.obe",
	"taskforce.obe",
	"minerbot.obe",
	"tribalguy.obe",
	"tribalchief.obe",
	NULL
};

char		lightingOn = 0;
char		showInvisibleObjects = FALSE;
char		graphOn = 0;
char		heapshow = 0;
bool		guiDebugDestruct = false;
bool		guiDebugFood = false;


#if BPLATFORM==PC
extern int bShowSetStrips;
#endif


/* --------------------------------------------------------------------------------
   Function : CreateMenuBar
   Purpose : create and setup the gui menu bar
   Parameters : 
   Returns : 
   Info : 
*/
void CreateMenuBar(void)
{
	int		i;

	mainMenu = bgCreateMenu(0);

 	// create file menu
	fileMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fileMenu,NULL,0,0,BGUIMENUITEMFLAG_SEPARATOR);
	bgAddMenuItem(fileMenu,"Exit",GUI_EXIT,0,0);
	bgAddMenuItem(fileMenu,__DATE__,GUI_EXIT,NULL,0);
	bgAddMenuItem(fileMenu,TAZ_VERSIONNUM,GUI_EXIT,NULL,0);


	// create video menu
	videoMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	NTSC = bgAddMenuItem(videoMenu,"NTSC - 60Hz",GUI_NTSC,0,BGUIMENUITEMFLAG_DISABLED);
	PAL = bgAddMenuItem(videoMenu,"PAL - 50Hz",GUI_PAL,0,
#if(BPLATFORM == XBOX)
		// PP: Aw look I'm tired and I want to get this disk done so I can go home.
		// PP: I'm sure they're perfectly capable of using the Xbox dashboard if they want to change video standard - that's what it's there for.
		BGUIMENUITEMFLAG_DISABLED);
#else// PP: not xbox
		0);
#endif// PP: not xbox

	// create view menu
	viewMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	ingameView = bgAddMenuItem(viewMenu,"Ingame View",GUI_INGAMEVIEW,0,BGUIMENUITEMFLAG_DISABLED);
	objectView = bgAddMenuItem(viewMenu,"Object Viewer",GUI_OBJECTVIEWER,0,0);
	guiFlyCam = bgAddMenuItem(viewMenu,"Fly Cam",GUI_FLYCAM,0,0);

	// create scene menu
	sceneMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	characterMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	enemyMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);

	// NH: Optimisation Menu
	optimisationMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	cartoonLighting = bgAddMenuItem(optimisationMenu,"Cartoon Lighting",GUI_CARTOONLIGHTING,0,0);
	outlines = bgAddMenuItem(optimisationMenu,"Outlines (Reloads Scene)",GUI_OUTLINES,0,0);
	incDrawDistance = bgAddMenuItem(optimisationMenu,"Increase Draw Distance",GUI_INCDRAWDISTANCE,0,0);
	decDrawDistance = bgAddMenuItem(optimisationMenu,"Decrease Draw Distance",GUI_DECDRAWDISTANCE,0,0);
	graph = bgAddMenuItem(optimisationMenu,"Toggle frame time graph",GUI_GRAPH,0,0);
	heap = bgAddMenuItem(optimisationMenu,"Show heap",GUI_HEAPSHOW,0,0);

	// NH: Tick menus item if pre-defined
	if (!(gameStatus.customOptions&CUSTOMOPTION_DISABLECARTOONLIGHTING))
		bgSetMenuItem(cartoonLighting,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);

	if (!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES))
		bgSetMenuItem(outlines,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);

	for (i=0;;i++)
	{
		if (characterNames[i]==NULL) break;
		bgAddMenuItem(characterMenu,characterNames[i],GUI_CHARTAZ+i,0,0);
	}

	// create options menu
	optionMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	
	guiDebugDPad			= bgAddMenuItem(optionMenu, "Debug DPad", GUI_DEBUGDPAD, 0, BGUIMENUITEMFLAG_TICKED);
	
	if (gameStatus.pad2Present)
	{
		multiplayerOption = bgAddMenuItem(optionMenu,"Multiplayer",GUI_MULTIPLAYER,0,0);
	}
	else
	{
		multiplayerOption = bgAddMenuItem(optionMenu,"Multiplayer",GUI_MULTIPLAYER,0,BGUIMENUITEMFLAG_DISABLED);
	}

	versusCPU				= bgAddMenuItem(optionMenu,"(Versus CPU)",GUI_VERSUSCPU,0,0);
	guiToggleSplitScreenMode= bgAddMenuItem(optionMenu,"(Toggle split-screen mode)",GUI_TOGGLESPLITSCREENMODE,0,0);
	m16x9					= bgAddMenuItem(optionMenu,"16:9",GUI_16x9,0,
#if((defined INFOGRAMES) || (!defined PHIL))/* PP: don't let Infogrames toggle aspect ratio other than from the Front End - I don't want any "bugs" coming back resulting from this*/
																		BGUIMENUITEMFLAG_DISABLED);
#else// PP: debug
																		0);
#endif// PP: debug

#if CHRISCAM_DEFAULTSTATE == 1
	cameraColl = bgAddMenuItem(optionMenu,"Chris Cam",GUI_CHRISCAM,0, BGUIMENUITEMFLAG_TICKED);// CPW: BGUIMENUITEMFLAG_TICKED);
#else
	cameraColl = bgAddMenuItem(optionMenu,"Chris Cam",GUI_CHRISCAM,0,0);// CPW: BGUIMENUITEMFLAG_TICKED);
#endif

	showInvisible = bgAddMenuItem(optionMenu,"Show Invisible",GUI_SHOWINVISIBLE,0,0);
	bgAddMenuItem(optionMenu,"Timer Bars",GUI_TIMER,0,0);

#if INSTANCENAME_DEFAULTSTATE == 1
	instanceName = bgAddMenuItem(optionMenu,"Instance Names",GUI_INSTANCENAME,0, BGUIMENUITEMFLAG_TICKED);
#else
	instanceName = bgAddMenuItem(optionMenu,"Instance Names",GUI_INSTANCENAME,0,0);
#endif

	debugDestruct = bgAddMenuItem(optionMenu,"Show Destructibles",GUI_DEBUGDESTRUCT,0, 0);
	debugFood = bgAddMenuItem(optionMenu,"Show Food",GUI_DEBUGFOOD,0, 0);

#if INSTANCEZONE_DEFAULTSTATE == 1
	instanceZone = bgAddMenuItem(optionMenu,"Instance Zones",GUI_INSTANCEZONE,0, BGUIMENUITEMFLAG_TICKED);
#else
	instanceZone = bgAddMenuItem(optionMenu,"Instance Zones",GUI_INSTANCEZONE,0,0);
#endif

	if (gameStatus.demoMode == -1)
		recordDemo = bgAddMenuItem(optionMenu,"Record Demo",GUI_RECORDDEMO,0, BGUIMENUITEMFLAG_TICKED);
	else
		recordDemo = bgAddMenuItem(optionMenu,"Record Demo",GUI_RECORDDEMO,0,0);

	if (gameStatus.demoMode == 1)
		playDemo = bgAddMenuItem(optionMenu,"Play Demo",GUI_PLAYDEMO,0, BGUIMENUITEMFLAG_TICKED);
	else
		playDemo = bgAddMenuItem(optionMenu,"Play Demo",GUI_PLAYDEMO,0,0);

	if (gameStatus.demoMode == 2)
		playHostDemo = bgAddMenuItem(optionMenu,"Play Host Demo",GUI_PLAYHOSTDEMO,0, BGUIMENUITEMFLAG_TICKED);
	else
		playHostDemo = bgAddMenuItem(optionMenu,"Play Host Demo",GUI_PLAYHOSTDEMO,0,0);

	// TP: create fog options
	fogMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);

	fogToggle = bgAddMenuItem(fogMenu,"Toggle Fog",GUI_FOGTOGGLE,0,0);

	fogMin = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fogMin,"Increase",GUI_FOGMININC,0,0);
	bgAddMenuItem(fogMin,"Decrease",GUI_FOGMINDEC,0,0);

	fogMax = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fogMax,"Increase",GUI_FOGMAXINC,0,0);
	bgAddMenuItem(fogMax,"Decrease",GUI_FOGMAXDEC,0,0);

	fogR = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fogR,"Increase",GUI_FOGRINC,0,0);
	bgAddMenuItem(fogR,"Decrease",GUI_FOGRDEC,0,0);

	fogG = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fogG,"Increase",GUI_FOGGINC,0,0);
	bgAddMenuItem(fogG,"Decrease",GUI_FOGGDEC,0,0);

	fogB = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(fogB,"Increase",GUI_FOGBINC,0,0);
	bgAddMenuItem(fogB,"Decrease",GUI_FOGBDEC,0,0);

	bgAddMenuItem(fogMenu,"Fog Min",0,fogMin,0);
	bgAddMenuItem(fogMenu,"Fog Max",0,fogMax,0);
	bgAddMenuItem(fogMenu,"Fog R",0,fogR,0);
	bgAddMenuItem(fogMenu,"Fog G",0,fogG,0);
	bgAddMenuItem(fogMenu,"Fog B",0,fogB,0);

	bgAddMenuItem(optionMenu,"Fog",0,fogMenu,0);

	// TP: create background menu
	backMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	
	backHeight = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(backHeight,"Increase",GUI_BACKHEIGHTINC,0,0);
	bgAddMenuItem(backHeight,"Decrease",GUI_BACKHEIGHTDEC,0,0);
	bgAddMenuItem(backMenu,"Background Height",0,backHeight,0);

	backAngle = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(backAngle,"Rotate Left",GUI_BACKROTLEFT,0,0);
	bgAddMenuItem(backAngle,"Rotate Right",GUI_BACKROTRIGHT,0,0);
	bgAddMenuItem(backMenu,"Background Y Rot",0,backAngle,0);

	bgAddMenuItem(optionMenu,"Background",0,backMenu,0);

	music = bgAddMenuItem(optionMenu,"Music",GUI_MUSIC,0, 0);

	console = bgAddMenuItem(optionMenu,"Debug Window",GUI_CONSOLE,0, 0);


	TBGuiElement *menu;
	char		 **name;
	int k;

	for (int j=0;j<4;j++)
	{
		menu = bgCreateMenu(BGUIMENUFLAG_POPUP);
		switch(j)
		{
		case 0:
			zooEnemyMenu = menu;
			name = zooEnemies;
			break;
		case 1:
			cityEnemyMenu = menu;
			name = cityEnemies;
			break;
		case 2:
			westEnemyMenu = menu;
			name = westEnemies;
			break;
		case 3:
			tazEnemyMenu = menu;
			name = tazEnemies;
			break;
		}
		i = 0;
		while (name[i]!=NULL)
		{
			k=0;
			while(characterNames[k]!=NULL)
			{
				if (strcmp(name[i],characterNames[k])==0)
				{
					bgAddMenuItem(menu, name[i], GUI_ENEMYTAZ+k,0,0);
					break;
				}
				k++;
			}
			i++;
		}
	}
	bgAddMenuItem(enemyMenu,"Zoo Enemies",0,zooEnemyMenu,0);
	bgAddMenuItem(enemyMenu,"City Enemies",0,cityEnemyMenu,0);
	bgAddMenuItem(enemyMenu,"West Enemies",0,westEnemyMenu,0);
	bgAddMenuItem(enemyMenu,"Taz Enemies",0,tazEnemyMenu,0);

	// PP: Create cheats menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// PP: Please be careful with the cheats menu, because I have to make some assumptions
	// PP: about the position at which each cheat name is inserted.
	// PP: I'm currently allowing for 1 item (qa cheats) before the ECheat members are listed.

	cheatMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);

	// TP: Add qa check first, so it doesn't get lost of bottom
	qaMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(cheatMenu,"Qa - cheats",0,qaMenu,0);
	bgAddMenuItem(qaMenu,"Finish Level",GUI_FINISHLEVEL,0,0);
	bgAddMenuItem(qaMenu,"Complete Posters",GUI_COMPLETEALLPOSTERS,0,0);
	bgAddMenuItem(qaMenu,"Force Win",GUI_FORCEWIN,0,0);
	bgAddMenuItem(qaMenu,"Force Loss",GUI_FORCELOSS,0,0);

	char				cheatName[CHEATS_MAX_NAME_LENGTH];		// PP: name of cheat
	int					flags;									// PP: flags used when adding the cheat to the menu
	TBStringTable*		tempStringTable;						// PP: ptr to string table for gui cheats menu


	// PP: **** define FORCE_ENGLISH_GUI to ensure that the gui cheats menu is created in English,	****
	// PP: **** regardless of the current language in effect at that point.							****
	// PP: NOTE: I fixed a bug where this would fail an ASSERT if the language was already English.
	// PP: It should work on any platform.
#if(BPLATFORM == XBOX || BPLATFORM == PC)// PP: dunno if you might need this as well Nige  // NH: Yep, looks like I do cheers
#define FORCE_ENGLISH_GUI
#endif// PP: xbox

#ifdef FORCE_ENGLISH_GUI

	if(gameStatus.currentLanguage != BLANGUAGEID_UK)
	{

		// PP: Set language to English a sec while we create the cheats list
		// PP: I don't want foreign cheat names in the GUI, not least because some foreign characters will crash it
		bkSetLanguage(BLANGUAGEID_UK);

		// PP: load temp English string table
		tempStringTable = LoadStringTable("main.txt","text");
	}
	else
	{
		tempStringTable=mainStringTable;
	}

#else// PP: ifdef FORCE_ENGLISH_GUI else

	tempStringTable=mainStringTable;

#endif// PP: def FORCE_ENGLISH_GUI else


	// PP: Add each of the cheat names, including the 'none' item
	for (i=0;i<NUM_CHEATS;i++)
	{
		bkString16to8(cheatName, tempStringTable->strings[CHEAT_NAME(i)].string);

		// PP: truncate the cheat string to fit in the GUI
		cheatName[GUI_MAX_STRING_LENGTH-1]='\0';

		// PP: determine flags to use when adding the cheat to the menu.

		if(guiCheatImplemented((ECheat)i))
		{
			flags=0;
		}
		else
		{
			flags=BGUIMENUITEMFLAG_DISABLED;
		}

		bgAddMenuItem(cheatMenu,cheatName,GUI_NOCHEATS+i,0,flags);

#ifndef HENRIK// PP: if all features are visible

		if(i == (FIRST_DEBUG_CHEAT-1))
		{
			bgAddMenuItem(cheatMenu,NULL,0,0,BGUIMENUITEMFLAG_SEPARATOR);
		}

#endif// PP: ndef HENRIK
	}

#ifdef FORCE_ENGLISH_GUI
	if(gameStatus.currentLanguage != BLANGUAGEID_UK)
	{
		// PP: A'ight set language back to what it should be (I set it to English while I create the cheats list)
		bkSetLanguage(gameStatus.currentLanguage);
		
		// PP: free temp English string table
		bkDeleteStringTable(tempStringTable);
	}
#endif// PP: def FORCE_ENGLISH_GUI

	bountyMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(cheatMenu,"Increase Bounty",0,bountyMenu,0);
	addBOne = bgAddMenuItem(bountyMenu,"$1",GUI_ADDBOUNTYONE,0,0);
	addBTen = bgAddMenuItem(bountyMenu,"$10",GUI_ADDBOUNTYTEN,0,0);
	addBHun = bgAddMenuItem(bountyMenu,"$100",GUI_ADDBOUNTYHUNDRED,0,0);
	addBTho = bgAddMenuItem(bountyMenu,"$1000",GUI_ADDBOUNTYTHOUSAND,0,0);
	addBTTh = bgAddMenuItem(bountyMenu,"$10000",GUI_ADDBOUNTYTENTHOU,0,0);
	addBHTh = bgAddMenuItem(bountyMenu,"$100000",GUI_ADDBOUNTYHUNTHOU,0,0);

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//create powerup menu
	powerUpMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	burpCan = bgAddMenuItem(powerUpMenu,"Burp Can",GUI_BURPCAN,0,0);
	hiccupCan = bgAddMenuItem(powerUpMenu,"Hiccup Can",GUI_HICCUPCAN,0,0);
	chilliPepper = bgAddMenuItem(powerUpMenu,"Chilli Pepper",GUI_CHILLIPEPPER,0,0);
	testTube = bgAddMenuItem(powerUpMenu,"Test Tube",GUI_TESTTUBE,0,0);
	bubblegum = bgAddMenuItem(powerUpMenu,"Bubblegum",GUI_BUBBLEGUM,0,0);

	// create main menu
	bgAddMenuItem(mainMenu,"File",0,fileMenu,0);
	bgAddMenuItem(mainMenu,"Video",0,videoMenu,0);
	bgAddMenuItem(mainMenu,"View",0,viewMenu,0);
	bgAddMenuItem(mainMenu,"Scene",0,sceneMenu,0);
#ifndef MARKETING
	bgAddMenuItem(mainMenu,"Options",0,optionMenu,0);
#endif
	bgAddMenuItem(mainMenu,"Power Ups",0,powerUpMenu,0);
	bgAddMenuItem(mainMenu,"Optimisation",0,optimisationMenu,0);	// NH: Add the optimisation menu (Hope I spelt that right)
	bgAddMenuItem(mainMenu,"Cheats",0,cheatMenu,0);					// PP: Add the cheats menu
#ifndef MARKETING
	bgAddMenuItem(mainMenu,"Character",0,characterMenu,0);
	bgAddMenuItem(mainMenu,"Create Enemy",0,enemyMenu,0);
#endif

	bgSetMainMenu(mainMenu);
}

/* --------------------------------------------------------------------------------
   Function : CreateSceneMenu
   Purpose : create and setup the scene menu bar
   Parameters : 
   Returns : 
   Info : 
*/
void CreateSceneMenu(void)
{
	TBPackageIndex		*pakIndex;
	int i, k = 0;
	char		tempName[256];

	miscLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Misc Levels",0,miscLevelMenu,0);
	zooLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Zoo Levels",0,zooLevelMenu,0);
	cityLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"City Levels",0,cityLevelMenu,0);
	westLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"West Levels",0,westLevelMenu,0);
	tazLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Taz Levels",0,tazLevelMenu,0);
	vrLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Vehicle Races",0,vrLevelMenu,0);
	deLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Destructions",0,deLevelMenu,0);
	taLevelMenu = bgCreateMenu(BGUIMENUFLAG_POPUP);
	bgAddMenuItem(sceneMenu,"Time Attacks",0,taLevelMenu,0);

	for (i = 0; i < 3; i++)
	{
		bgAddMenuItem(miscLevelMenu, miscLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 5; i++)
	{
		bgAddMenuItem(zooLevelMenu, zooLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 5; i++)
	{
		bgAddMenuItem(cityLevelMenu, cityLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 5; i++)
	{
		bgAddMenuItem(westLevelMenu, westLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 3; i++)
	{
		bgAddMenuItem(tazLevelMenu, tazLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 3; i++)
	{
		bgAddMenuItem(vrLevelMenu, vrLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 3; i++)
	{
		bgAddMenuItem(deLevelMenu, deLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = 0; i < 3; i++)
	{
		bgAddMenuItem(taLevelMenu, taLevels[i], GUI_SCENESELECT+k,0,0);
		k++;
	}

	for (i = SCENE_SAFARIRACE + 1; i < ENDOFSCENES; i++)
	{
		bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+k,0,0);
		k++;
	}

	/*for (i=0;i<ENDOFSCENES;i++)
	{
#ifdef MARKETING
		if (i<=SCENE_ZOOBOSS)
#endif
		{
			if (i <= SCENE_SAFARIRACE)
			{
				// check packfile is there
				if (masterPakfile)
				{
	#if BPLATFORM == PS2
					sprintf(tempName,"%s.ps2",sceneFileName[i]);
	#else
					sprintf(tempName,"%s.pc",sceneFileName[i]);
	#endif
					if (bkFindFilenameByCRC(bkGetPackageID(masterPakfile),bkCRC32((uchar*)tempName,strlen(tempName),0)))
					{
						bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+i,0,0);
					}
					else
					{
						bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+i,0,BGUIMENUITEMFLAG_DISABLED);
					}
				}
				else
				{
					if (pakIndex = OpenPackage(sceneFileName[i]))
					{
						bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+i,0,0);
						bkClosePackage(pakIndex);
					}
					else
					{
						bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+i,0,BGUIMENUITEMFLAG_DISABLED);
					}
				}
			}
			else
			{
				bgAddMenuItem(sceneMenu,sceneText[i],GUI_SCENESELECT+i,0,0);
			}
		}
	}*/
}

/*  --------------------------------------------------------------------------------
	Function	: DisplayGuiInfo
	Purpose		: Print any gui information to the screen
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DisplayGuiInfo(void)
{
	if (guiTime < 10.0f)
	{
		char	buf[256];
		ushort	string16[256];

		guiTime+=fTime;

		// TP: print info
		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	#if BPLATFORM == PS2
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	#endif
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		TextFX_vGrad(128,128,0,110,40,0);

		sprintf(buf, "Fog:min %d, max %d, RGB(%d,%d,%d)", (int)UNITTOMETER(map.fogMin), (int)UNITTOMETER(map.fogMax), map.fogColour.r, map.fogColour.g, map.fogColour.b);
		bkString8to16(string16, buf);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen+60)/2),(float)(-(videoMode.yScreen)/2)+40, 128,128,128, 128, TextFX_escapeCallback, NULL);
		sprintf(buf, "Bckg:Height %d, Angle %d", (int)UNITTOMETER(map.backgroundYOffset), (int)DEG(map.backgroundAngle));
		bkString8to16(string16, buf);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen+60)/2),(float)(-(videoMode.yScreen)/2)+80, 128,128,128, 128, TextFX_escapeCallback, NULL);

		
		TextFX_resetColour();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	}
}

/* --------------------------------------------------------------------------------
   Function : GuiCallback
   Purpose : called when gui is active and a selection has been made
   Parameters : TBGuiEventInfo *cbInfo
   Returns : 
   Info : 
*/
int GuiCallback(TBGuiEventInfo *cbInfo)
{
	TBActor		*actor;
	char		tempName[256];
	char		*temp;
	TBActorNodeInstance	*node;
	TBVector	offset;
	static		bool debugConsole = false;

    switch(cbInfo->reason)
    {
	case EBGUIEVENT_MENUSELECT:
		switch(cbInfo->resourceID)
		{
		case GUI_FINISHLEVEL:
			FinishLevel(map.sceneNumber);
			return true;

		case GUI_COMPLETEALLPOSTERS:
			FinishLevel(map.sceneNumber, false);
			return true;

		case GUI_FORCEWIN:
			ForceWin(map.sceneNumber);
			return true;

		case GUI_FORCELOSS:
			ForceLoss(map.sceneNumber);
			return true;

		case GUI_CONSOLE:
			if (debugConsole)
			{
				bdShowConsoleWindow(false);
				bdEnableConsoleWindow(false);
				debugConsole = false;
				bgSetMenuItem(console,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				SetupConsole();
				debugConsole = true;
				bdShowConsoleWindow(true);
				bgSetMenuItem(console,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			return true;

		case GUI_MUSIC:
			if (gameStatus.enableMusic)
			{
				bgSetMenuItem(music,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
				gameStatus.enableMusic=false;
//				musicSystem.FadeAndFreeAll(0.0f);
			}
			else
			{
				bgSetMenuItem(music,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				gameStatus.enableMusic=true;
//				musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_MAIN,MUSICSTATE_START);
			}
			return true;
		case GUI_BACKHEIGHTINC:
			map.backgroundYOffset += METERTOUNIT(1.0f);
			SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset);
			guiTime = 0.0f;
			return 0;
		case GUI_BACKHEIGHTDEC:
			map.backgroundYOffset -= METERTOUNIT(1.0f);
			SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset);
			guiTime = 0.0f;
			return 0;
		case GUI_BACKROTLEFT:
			map.backgroundAngle -= RAD(1.0f);
			if (map.backgroundAngle < 0.0f) map.backgroundAngle += RAD(360.0f);
			SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset);
			guiTime = 0.0f;
			return 0;
		case GUI_BACKROTRIGHT:
			map.backgroundAngle += RAD(1.0f);
			if (map.backgroundAngle > RAD(360.0f)) map.backgroundAngle -= RAD(360.0f);
			SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset);
			guiTime = 0.0f;
			return 0;
		case GUI_FOGMININC:
			if (map.fogMin < (map.fogMax-METERTOUNIT(5.0f)))
			{
				map.fogMin += METERTOUNIT(5.0f);
				//map.fogMin = map.fogMinOrig + map.fogMin + METERTOUNIT(5.0f);
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGMINDEC:
			if (map.fogMin > 0.0f)// PP: was METERTOUNIT(5.0f)
			{
				map.fogMin -= METERTOUNIT(5.0f);
				//map.fogMin = map.fogMinOrig + map.fogMin - METERTOUNIT(5.0f);

				map.fogMin=MAX(map.fogMin, 0.0f);// PP: 
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);
			
			guiTime = 0.0f;
			return 0;
		case GUI_FOGMAXINC:
			if (map.fogMax < METERTOUNIT(300.0f))
			{
				map.fogMax += METERTOUNIT(5.0f);
				//map.fogMax = map.fogMaxOrig + map.fogMax + METERTOUNIT(5.0f);
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGMAXDEC:
			if (map.fogMax > (map.fogMin+METERTOUNIT(5.0f)))
			{
				map.fogMax -= METERTOUNIT(5.0f);
				//map.fogMax = map.fogMaxOrig + map.fogMax - METERTOUNIT(5.0f);
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGRINC:
			if (map.fogColour.r < 255)
			{
				map.fogColour.r += 5;
			}
			else
			{
				map.fogColour.r = 255;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGRDEC:
			if (map.fogColour.r > 0)
			{
				map.fogColour.r -= 5;
			}
			else
			{
				map.fogColour.r = 0;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGGINC:
			if (map.fogColour.g < 255)
			{
				map.fogColour.g += 5;
			}
			else
			{
				map.fogColour.g = 255;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGGDEC:
			if (map.fogColour.g > 0)
			{
				map.fogColour.g -= 5;
			}
			else
			{
				map.fogColour.g = 0;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGBINC:
			if (map.fogColour.b < 255)
			{
				map.fogColour.b += 5;
			}
			else
			{
				map.fogColour.b = 255;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGBDEC:
			if (map.fogColour.b > 0)
			{
				map.fogColour.b -= 5;
			}
			else
			{
				map.fogColour.b = 0;
			}
			
			if (map.fogState)
				SetFog(map.fogMin,map.fogMax,map.fogColour);

			guiTime = 0.0f;
			return 0;
		case GUI_FOGTOGGLE:
			if (!map.fogState)
			{
				map.fogState = 1;
				SetFog(map.fogMin,map.fogMax,map.fogColour);
			}
			else
			{
				map.fogState = 0;
			}			
			return 0;
			break;
		case GUI_EXIT:
			return 1;
			break;
		case GUI_NTSC:
			bgSetMenuItem(PAL,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
			bgSetMenuItem(NTSC,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
			videoMode.flagRequest = videoMode.flags;
			videoMode.flagRequest |= BDISPLAYFLAG_NTSC;
			videoMode.flagRequest &= ~BDISPLAYFLAG_PAL;

// PP: in NTSC, we're gonna be in a low enough res for antialising
#if(BPLATFORM == XBOX)
			videoMode.flagRequest |= BDISPLAYFLAG_ANTIALIASED;
#endif// PP: xbox

			guiChangeScene(gameStatus.currentScene);
			return 1;
			break;
		case GUI_PAL:
			bgSetMenuItem(NTSC,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
			bgSetMenuItem(PAL,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
			videoMode.flagRequest = videoMode.flags;
			videoMode.flagRequest |= BDISPLAYFLAG_PAL;
			videoMode.flagRequest &= ~BDISPLAYFLAG_NTSC;

// PP: in PAL, on Xbox, we're gonna be in too high a res for antialising
#if(BPLATFORM == XBOX)
			videoMode.flagRequest &= ~BDISPLAYFLAG_ANTIALIASED;
#endif// PP: xbox

			guiChangeScene(gameStatus.currentScene);
			return 1;
			break;
		case GUI_OBJECTVIEWER:
			bgSetMenuItem(ingameView,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
			bgSetMenuItem(objectView,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
			setFOV(OBJECT_VIEWER_FOV);
			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
			SetGameState(&gameStatus,GAMESTATE_OBJECTVIEWER,GAMESUBSTATE_NORMAL);
			InitObjectViewer();
			return 1;
			break;
		case GUI_INGAMEVIEW:
			bgSetMenuItem(objectView,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
			bgSetMenuItem(ingameView,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
			SetGameState( &gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL );
			setFOV(NORMAL_INGAME_FOV);
			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
			FreeObjectViewer();
			return 1;
			break;
		case GUI_FLYCAM:
			{
				toggleFlyCam();
				bgSetMenuItem(guiFlyCam,NULL,NULL,NULL,flyCamActive?BGUIMENUITEMFLAG_TICKED:0,BGUIMENUITEMFLAG_TICKED);
			}
			return 1;;
		case GUI_CHRISCAM:

			if(gameStatus.chrisCam == 0)
			{
				gameStatus.chrisCam = 1;

				bgSetMenuItem(cameraColl,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				gameStatus.chrisCam = 0;

				bgSetMenuItem(cameraColl,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}

			return 1;
			break;

		case GUI_INSTANCENAME:

			if(gameStatus.instanceName == 0)
			{
				gameStatus.instanceName = 1;

				bgSetMenuItem(instanceName,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				gameStatus.instanceName = 0;

				bgSetMenuItem(instanceName,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}

			return 1;
			break;

		case GUI_DEBUGDESTRUCT:
			if(guiDebugDestruct == 0)
			{
				guiDebugDestruct = 1;

				bgSetMenuItem(debugDestruct,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				guiDebugDestruct = 0;

				bgSetMenuItem(debugDestruct,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}

			return 1;
			break;

		case GUI_DEBUGFOOD:
			if(guiDebugFood == 0)
			{
				guiDebugFood = 1;

				bgSetMenuItem(debugFood,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				guiDebugFood = 0;

				bgSetMenuItem(debugFood,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}

			return 1;
			break;

		case GUI_INSTANCEZONE:

			if(gameStatus.instanceZone == 0)
			{
				gameStatus.instanceZone = 1;

				bgSetMenuItem(instanceZone,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				gameStatus.instanceZone = 0;

				bgSetMenuItem(instanceZone,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}

			return 1;
			break;

		case GUI_RECORDDEMO:	
			
			if(gameStatus.demoMode == -1)
			{
				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);

				bgSetMenuItem(recordDemo,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				if (!gameStatus.demoMode)
				{
					gameStatus.demoMode = -1;

					camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);
				
					bgSetMenuItem(recordDemo,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				}
			}
			
			return 1;
			break;
		
		case GUI_PLAYDEMO:		
			
			if(gameStatus.demoMode == 1)
			{
				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_FRONTEND, 1.0f);

				bgSetMenuItem(playDemo,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				if (!gameStatus.demoMode)
				{
					bkPrintf("demoMode set to Disk Play through GUI");

					gameStatus.demoMode = 1;

					camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);
				
					bgSetMenuItem(playDemo,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				}
			}
			
			return 1;
			break;

		case GUI_PLAYHOSTDEMO:		
			
			if(gameStatus.demoMode == 2)
			{
				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);

				bgSetMenuItem(playHostDemo,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				if (!gameStatus.demoMode)
				{
					gameStatus.demoMode = 2;

					camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);
				
					bgSetMenuItem(playHostDemo,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				}
			}
			
			return 1;
			break;

		case GUI_DEBUGDPAD:		
			
			if(debugDPad)
			{
				debugDPad = 0;

				bgSetMenuItem(guiDebugDPad,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);

				// TP: need to delete the old channels first

				FreeRumbleController(&controller1);
				biResetMap(controller1.inputMap);
				FreeRumbleController(&controller2);
				biResetMap(controller2.inputMap);
				
				CreateInputChannels(&controller1,0);
				if (BindInputChannels(&controller1,0)) gameStatus.pad1Present = TRUE;
				CreateInputChannels(&controller2,1);
				if (BindInputChannels(&controller2,1)) gameStatus.pad2Present = TRUE;
			}
			else
			{
				debugDPad = 1;
				
				bgSetMenuItem(guiDebugDPad,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				
				FreeRumbleController(&controller1);
				biResetMap(controller1.inputMap);
				FreeRumbleController(&controller2);
				biResetMap(controller2.inputMap);
				
				CreateInputChannels(&controller1,0);
				if (BindInputChannels(&controller1,0)) gameStatus.pad1Present = TRUE;
				CreateInputChannels(&controller2,1);
				if (BindInputChannels(&controller2,1)) gameStatus.pad2Present = TRUE;
			}
			
			return 1;
			break;

		case GUI_SHOWINVISIBLE:
			if (showInvisible)
			{
				bgSetMenuItem(showInvisible,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
				showInvisibleObjects = 1 - showInvisibleObjects;
			}
			else
			{
				bgSetMenuItem(showInvisible,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				showInvisibleObjects = 1 - showInvisibleObjects;
			}
			return 1;
			break;

		case GUI_MULTIPLAYER:
			if(gameStatus.multiplayer.numPlayers == 1)
			{
				bgSetMenuItem(multiplayerOption,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				gameStatus.multiplayer.numPlayers = 2;

				if(gameStatus.multiplayer.versusCPU)
					setNumScreens(1);
				else
					setNumScreens(2);
			}
			else
			{
				bgSetMenuItem(multiplayerOption,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
				gameStatus.multiplayer.numPlayers = 1;
				setNumScreens(1);
			}

			camera[0].fade.SetSwipeDirection(PI/2,-PI/2);
			camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,gameStatus.currentScene,0.5);
			SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
			return 1;
			break;

		case GUI_VERSUSCPU:
			if(gameStatus.multiplayer.versusCPU == 1)
			{
				bgSetMenuItem(versusCPU,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
				gameStatus.multiplayer.versusCPU = 0;
			}
			else
			{
				bgSetMenuItem(versusCPU,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
				gameStatus.multiplayer.versusCPU = 1;
			}

			return 0;
			break;

		case GUI_TOGGLESPLITSCREENMODE:
			{
				toggleSplitScreenMode();
			}
			break;

		case GUI_16x9:
			{
				// PP: this sets gameStatus.wideScreenWilson and videoMode.xAspect&yAspect + ticks/unticks gui item
				toggleWidescreen();
			}
			// PP: REMOUT: it's nice to be able to toggle this repeatedly to look at differences between aspect ratios			return 1;
			break;

		case GUI_CARTOONLIGHTING:
			// NH: If cartoon lighting is set off (CUSTOMOPTION_DISABLECARTOONLIGHTING == TRUE)
			if ((gameStatus.customOptions&CUSTOMOPTION_DISABLECARTOONLIGHTING))
			{
				// NH: Set cartoon lighting on
				gameStatus.customOptions &= ~CUSTOMOPTION_DISABLECARTOONLIGHTING;				
				
				if (gameStatus.player1!=NULL)
					EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);
				else if (gameStatus.player2!=NULL)
					EnableActorCartoonRenderMode(&gameStatus.player2->actorInstance);
				
				bgSetMenuItem(cartoonLighting,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			// NH: If cartoon lighting is set on (CUSTOMOPTION_DISABLECARTOONLIGHTING == FALSE)
			{
				// NH: Set cartoon lighting off
				gameStatus.customOptions |= CUSTOMOPTION_DISABLECARTOONLIGHTING;
				
				if (gameStatus.player1!=NULL)
					DisableActorCartoonRenderMode(&gameStatus.player1->actorInstance);
				else if (gameStatus.player2!=NULL)
					DisableActorCartoonRenderMode(&gameStatus.player2->actorInstance);
				
				// NH: If cartoon outline is set on (CUSTOMOPTION_DISABLEOUTLINES == FALSE)
				if (!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES))
				{
					// NH: turn outline back on
					if (gameStatus.player1!=NULL)
						EnableActorLineRenderMode(&gameStatus.player1->actorInstance);
					else if (gameStatus.player2!=NULL)
						EnableActorLineRenderMode(&gameStatus.player2->actorInstance);
				}

				bgSetMenuItem(cartoonLighting,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
			}
			return 1;
			break;

		case GUI_OUTLINES:
			// NH: If cartoon outline is set off (CUSTOMOPTION_DISABLEOUTLINES == TRUE)
			if ((gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES))
			{
				// NH: Set cartoon outlines on
				gameStatus.customOptions &= ~CUSTOMOPTION_DISABLEOUTLINES;
				// NH: Reload scene
				ChangeScene(&gameStatus, gameStatus.currentScene);
				bgSetMenuItem(outlines,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
			}
			else
			{
				// NH: Set cartoon outlines off
				gameStatus.customOptions |= CUSTOMOPTION_DISABLEOUTLINES;
				// NH: Reload scene
				ChangeScene(&gameStatus, gameStatus.currentScene);
				bgSetMenuItem(outlines,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);		
			}
			return 1;
			break;
		
		case GUI_INCDRAWDISTANCE:
			if (videoMode.farPlane < METERTOUNIT(300.0f))
			{
				// NH: Move far (clipping) plane out
				videoMode.farPlane += METERTOUNIT(5.0f);

				// NH: Move fogging region in proportion to the far plane
				map.fogMin = (map.fogMinOrig*(videoMode.farPlane / METERTOUNIT(300.0f)));
				map.fogMax = (map.fogMaxOrig*(videoMode.farPlane / METERTOUNIT(300.0f)));
				//map.fogMax += METERTOUNIT(5.0f);
				//map.fogMin += METERTOUNIT(5.0f);

				// Setup environment map
				if (SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset)==FALSE)
				{
					bkPrintf("*** WARNING *** No background specified or textures %s, %s and %s not present\n",map.background1,map.background2,map.background3);
					SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,"main.bmp","second.bmp",NULL,map.backgroundAngle,map.backgroundYOffset);
				}

				if (map.fogState)
					SetFog(map.fogMin,map.fogMax,map.fogColour);

			}			
			return 0;
			break;
		
		case GUI_DECDRAWDISTANCE:
			if (videoMode.farPlane >= METERTOUNIT(40.0f))
			{
				// NH: Move far (clipping) plane in
				videoMode.farPlane -= METERTOUNIT(5.0f);
				
				// NH: Move fogging region in proportion to the far plane
				map.fogMin = (map.fogMinOrig*(videoMode.farPlane / METERTOUNIT(300.0f)));
				map.fogMax = (map.fogMaxOrig*(videoMode.farPlane / METERTOUNIT(300.0f)));
//				map.fogMax -= METERTOUNIT(5.0f);
//				map.fogMin -= METERTOUNIT(5.0f);

				// Setup environment map
				if (SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset)==FALSE)
				{
					bkPrintf("*** WARNING *** No background specified or textures %s, %s and %s not present\n",map.background1,map.background2,map.background3);
					SetupEnvMapSpecial(&environmentMap,videoMode.farPlane/2.0f,"main.bmp","second.bmp",NULL,map.backgroundAngle,map.backgroundYOffset);
				}

				if (map.fogState)
					SetFog(map.fogMin,map.fogMax,map.fogColour);

			}	
			return 0;
			break;
		
		case GUI_TIMER:
			showTimerBars = 1-showTimerBars;
			return 1;
		case GUI_BURPCAN:
			if (gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_SPIN)
			{
				node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,"spinelower");
				PlayAnimationByNamePart(gameStatus.player[0].actorInstance,"runeat2",1.0f,0,0,0.1f,MOVE,node);
				baQueueNodeAnimBlendToNode(&gameStatus.player[0].actorInstance->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,0.1f);
			}
			if (gameStatus.player[0].actorInstance->characterInfo)
			{
				IncreaseCharactersBurp(gameStatus.player[0].actorInstance->characterInfo,1);
			}
			return 1;
		case GUI_HICCUPCAN:
			if (gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_SPIN)
			{
				node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,"spinelower");
				PlayAnimationByNamePart(gameStatus.player[0].actorInstance,"runeat2",1.0f,0,0,0.1f,MOVE,node);
				baQueueNodeAnimBlendToNode(&gameStatus.player[0].actorInstance->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,0.1f);
			}
			if(gameStatus.player[0].actorInstance->characterInfo)
			{
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.hiccup = TRUE;
			}
			return 1;
		case GUI_CHILLIPEPPER:
			if (gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_SPIN)
			{
				node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,"spinelower");
				PlayAnimationByNamePart(gameStatus.player[0].actorInstance,"runeat2",1.0f,0,0,0.1f,MOVE,node);
				baQueueNodeAnimBlendToNode(&gameStatus.player[0].actorInstance->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,0.1f);
			}
			if(gameStatus.player[0].actorInstance->characterInfo)
			{
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.state = POWERUPSTATE_CHILLIOFF;
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.chilliPepper = TRUE;
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.numFire = CHILLIPEPPER_FIRE_NUM;
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.clock = 20.0f;
				RequestStateChange(gameStatus.player[0].actorInstance, STATE_CHILLIPEPPER);
			}
			return 1;
		case GUI_TESTTUBE:
			if (gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_SPIN)
			{
				node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,"spinelower");
				PlayAnimationByNamePart(gameStatus.player[0].actorInstance,"runeat2",1.0f,0,0,0.1f,MOVE,node);
				baQueueNodeAnimBlendToNode(&gameStatus.player[0].actorInstance->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,0.1f);
			}
			if(gameStatus.player[0].actorInstance->characterInfo)
			{
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.invisible= TRUE;
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.thirdClock = INVISIBLE_INITIAL_TIME_DIFF;
				EnableActorLineOnlyRenderMode(&gameStatus.player[0].actorInstance->actorInstance, &invisLineParms);
				// NH: baSetActorRenderMode(&gameStatus.player[0].actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
				// NH: baSetActorRenderMode(&gameStatus.player[0].actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
				gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.state = RENDERSTATE_LINEONLY;
			}
			return 1;
		case GUI_BUBBLEGUM:
			if (gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_SPIN)
			{
				node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,"spinelower");
				PlayAnimationByNamePart(gameStatus.player[0].actorInstance,"runeat2",1.0f,0,0,0.1f,MOVE,node);
				baQueueNodeAnimBlendToNode(&gameStatus.player[0].actorInstance->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance,0.1f);
			}
			if(gameStatus.player[0].actorInstance->characterInfo)
			{
				RequestStateChangePlus(gameStatus.player[0].actorInstance, STATE_BUBBLEGUM, myStateChangeBubble);
			}
			return 1;
		case GUI_GRAPH:
			graphOn ^= 1;
#if BPLATFORM != PS2
			bdEnableFrameTimeGraph(graphOn);
			bdEnableMemoryGraph(graphOn);
#endif
			return 1;

		case GUI_HEAPSHOW:
			heapshow ^= 1;
			return 1;
		
		case GUI_ADDBOUNTYONE:		AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 1);
									return 1;
		case GUI_ADDBOUNTYTEN:		AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 10);
									return 1;
		case GUI_ADDBOUNTYHUNDRED:	AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 100);
									return 1;
		case GUI_ADDBOUNTYTHOUSAND:	AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 1000);
									return 1;
		case GUI_ADDBOUNTYTENTHOU:	AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 10000);
									return 1;
		case GUI_ADDBOUNTYHUNTHOU:	AddMoneyToBounty(gameStatus.player[0].actorInstance, NULL, 100000);
									return 1;
		}

		// PP: Handle choosing a cheat
		if ((cbInfo->resourceID>=GUI_NOCHEATS)&&(cbInfo->resourceID < GUI_LASTCHEAT))
		{
			// PP: if we've chosen 'none'...
			if(cbInfo->resourceID == GUI_NOCHEATS)
			{
				// PP: ...deactivate all cheats
				DeactivateAllCheats();
			}
			else
			{
				// PP: ...otherwise toggle the cheat on / off

				ECheat		cheat=(ECheat)(cbInfo->resourceID-GUI_NOCHEATS);

				ToggleCheat(cheat);
			}

			return 1;
		}

		if ((cbInfo->resourceID>=GUI_ENEMYTAZ)&&(cbInfo->resourceID <= 150))
		{
			guiCreateEnemy("guienemy", NULL,characterNames[cbInfo->resourceID-100],NULL,NULL,NULL,NULL,0,false);
			return 1;
		}
		if ((cbInfo->resourceID>=GUI_CHARTAZ)&&(cbInfo->resourceID <= 100))
		{
			actor = FindActorInActorList(characterNames[cbInfo->resourceID-50]);
			if (!actor) 
			{
				strcpy(tempName,characterNames[cbInfo->resourceID-50]);

				temp = FindChar(tempName,'.');
				if (temp!=NULL) *temp = 0;

				LoadWholePackage(tempName,1);
				LoadActor(characterNames[cbInfo->resourceID-50],tempName);
			}

			ChangeActorInstance(gameStatus.player1,FindActorInActorList(characterNames[cbInfo->resourceID-50]),5,NULL);
			return 1;
		}
		if ((cbInfo->resourceID>=GUI_SCENESELECT)&&(cbInfo->resourceID<GUI_SCENESELECT+ENDOFSCENES))
		{
			// TP: Select level change
			if (map.sceneNumber == SCENE_FRONTEND)// PP: if we're in the front-end...
			{
				// PP: ...request the scene change from the front-end code - this gives us a chance to save off the global game settings...
// TP: 				FrontEnd_startSceneChange(cbInfo->resourceID - GUI_SCENESELECT);
				guiChangeScene(cbInfo->resourceID - GUI_SCENESELECT);
			}
			else
			{
				// PP: ...otherwise change scene as normal.
				guiChangeScene(cbInfo->resourceID - GUI_SCENESELECT);
			}
			return 1;
		}
    }

    return 0;
}

/* --------------------------------------------------------------------------------
   Function : guiChangeScene
   Purpose : change scene
   Parameters : index of scene to change to
   Returns : 
   Info : // PP: this is needed by the front-end, which has to save global game settings before exiting
*/
void guiChangeScene(const int sceneIndex)
{
 	//musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_ALL,MUSICSTATE_STOP);

	if (gameStatus.globalSettings.currentSaveSlot==-1)
	{
		// TP: must choose slot for debug change
		gameStatus.globalSettings.currentSaveSlot = 0;
		if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].inUse)
		{
			InitialiseSaveGameSlot(gameStatus.globalSettings.currentSaveSlot,gameStatus.globalSettings.difficulty);
		}
	}

	if ((map.sceneNumber==SCENE_LANGUAGESELECT)&&(map.lastScene != SCENE_FRONTEND))
	{
		CheckMemCardOnBoot();
	}

	camera[0].fade.SetSwipeDirection(PI/2,-PI/2);
	map.startPos = 0;
	map.startPos2 = 0;
	gameStatus.multiplayer.versusCPU = true;
	camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,sceneIndex,0.5);
	SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
}


/* --------------------------------------------------------------------------------
   Function : guiCreateEnemy
   Purpose : create and setup an enemy for testing
   Parameters : name of enemy obe
   Returns : PP: ptr to newly-created enemy ACTORINSTANCE
   Info : 
*/

ACTORINSTANCE* guiCreateEnemy(char *instanceName, char *bankObject, char *name,TBVector position,TBQuaternion orientation, TBVector scale,uint32 attackPointCRC,int type,bool box, bool active, uint32 cageCRC)
{
	TBActor			*actor;
	TBVector		tempVector, zeroVec;
	ACTORINSTANCE	*tempInstance,*tempInstance2;
	char			tempName[256],soundName[256];
	char			*temp;
	bool			done=false;

	//if ((strcmpi(bankObject, "catcher.obe")==0)&&(map.sceneNumber==SCENE_GOLDMINE)) return false;

	if (position)
	{
		bmVectorCopy(tempVector,position);
	}
	else
	{
		bmVectorSub(tempVector,gameStatus.player[0].actorInstance->actorInstance.position,gameStatus.player[0].camera->pos);
		tempVector[Y] = 0.0f;
		bmVectorScaleToLength(tempVector,tempVector,METERTOUNIT(5.0f));
		bmVectorAdd(tempVector,tempVector,gameStatus.player[0].actorInstance->actorInstance.position);
	}

	if (strcmpi(name,"phonebox.obe")==0)
	{
		tempInstance = CreateActorInstance(&map.phoneBoxInstances,"phonebox.obe","phonebox");
		tempInstance2 = CreateActorInstance(&map.invisibleInstances,"phonebox_collision.obe","phoneboxcoll");

		bmVectorCopy(tempInstance->actorInstance.position,tempVector);

		bmVectorCopy(tempInstance2->actorInstance.position,tempVector);
		
		CreateActorInstancePhoneBox(tempInstance,tempInstance2,BOXTYPE_POLICE);

		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		return tempInstance;
	}

	// TP: convert museum guard into green guard
	if (((map.sceneNumber==SCENE_DEPTSTR)||(map.sceneNumber==SCENE_DEMODEPT))&&(strcmpi(name,"guard.obe")==0))
	{
		name = "greenguard.obe";
	}

	// TP: convert bjshellac into scargill on the gold mine
	if ((map.sceneNumber==SCENE_GOLDMINE)&&(strcmpi(name,"bjshellac.obe")==0))
	{
		name = "bjscargill.obe";
	}

	// TP: convert all the other guardians into the keeper
	if ((strcmpi(name,"catcher.obe")==0)||(strcmpi(name,"taskforce.obe")==0)||(strcmpi(name,"anvilgang.obe")==0))
	{
		name = "keeper.obe";
	}

	actor = FindActorInActorList(name);

	strcpy(tempName,name);
	temp = FindChar(tempName,'.');
	if (temp!=NULL) *temp = 0;
	
	if (!actor) 
	{
#ifdef NOAUTOLOADENEMIES	
		return NULL;
#endif

#ifndef NOAUTOLOADENEMIES
		LoadWholePackage(tempName,1);
		LoadActor(name,tempName);

		// TP: make copy because I still need tempName later
		strcpy(soundName,tempName);
		LoadCharacterSamplesBkg(soundName);

		// PP: set up and open the books to be displayed during loading
		openLoadingBooks();

		while (GetCurrentBkgLoadingStatus()==TRUE)
		{
			DrawLoadLoop(0);
		}

		// PP: close the books that were displayed during loading
		closeLoadingBooks();

		LoadCharacterSamples(soundName);
#endif
	}

	tempInstance = CreateActorInstance(&map.characterInstances,name,instanceName,tempVector);
	if (!tempInstance) return NULL;// PP: if the ACTORINSTANCE couldn't be created, just return NULL

	tempInstance->special = SPECIALTYPE_ENEMY;
	ENABLE_LINE_OR_CARTOON_RENDER_MODE(&tempInstance->actorInstance);
	SetActorPosition(tempInstance,tempVector);
	
	if (strcmpi(name,"keeper.obe")==0)
	{
		CreateActorInstanceZookeeper(tempInstance,&map.characterInstances,&map.inactiveCharacterInstances,(EZookeeperType)type);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if((strcmpi(name, "doggy.obe")==0)||(strcmpi(name, "demondog.obe")==0))
	{
		int type;
		if (strcmpi(name, "doggy.obe")==0)
		{
			type = DOGGY;
		}
		else
		{
			type = DEMONDOG;
		}
		CreateActorInstanceDoggy(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances,type);
		done=true;
	}
	if((strcmpi(name, "guard.obe")==0)||(strcmpi(name, "greenguard.obe")==0))
	{
		CreateActorInstanceMuseumGuard(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
#ifndef CONSUMERDEMO
	if (strcmpi(name,"alligator.obe")==0)
	{
		SETVECTOR(zeroVec, 0.0f, 0.0f, 0.0f, 1.0f);
		if (!attackPointCRC)
		{
			attackPointCRC = bkCRC32((uchar*)"signpost_i01",strlen("signpost_i01"),0);
		}
		CreateActorInstanceAlligator(tempInstance,&map.characterInstances,&map.inactiveCharacterInstances,attackPointCRC);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if (strcmpi(name,"polarbear.obe")==0)
	{
		CreateActorInstanceBear(tempInstance,&map.characterInstances,&map.inactiveCharacterInstances,0);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if (strcmpi(name,"browbear.obe")==0)
	{
		CreateActorInstanceBear(tempInstance,&map.characterInstances,&map.inactiveCharacterInstances,1);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if(strcmpi(name, "catcher.obe")==0)
	{
		CreateActorInstanceTazCatcher(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if(strcmpi(name, "security bot.obe")==0)
	{
		CreateActorInstanceSecurityBot(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if(strcmpi(name, "pirateape.obe")==0)
	{
		CreateActorInstanceApe(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if(strcmpi(name, "shark.obe")==0)
	{
		CreateActorInstanceShark(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
		done=true;
	}
	if(strcmpi(name, "construction bot.obe")==0)
	{
		CreateActorInstanceConstructionBot(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if(strcmpi(name, "construction worker.obe")==0)
	{
		CreateActorInstanceConstructionWorker(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if(strcmpi(name, "taskforce.obe")==0)
	{
		CreateActorInstanceTaskForce(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if(strcmpi(name, "anvilgang.obe")==0)
	{
		LoadActor("anvil.obe",tempName);
		CreateActorInstanceAnvilGang(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if((strcmpi(name, "bjshellac.obe")==0)||(strcmpi(name, "bjscargill.obe")==0))
	{
		LoadActor("bjdynamite.obe",tempName);
		int type;
		if (strcmpi(name, "bjshellac.obe")==0)
		{
			type = 0;
		}
		else
		{
			type = 1;
		}
		CreateActorInstanceBJShellac(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances,type);
		done=true;
	}
	if(strcmpi(name, "tribalchief.obe")==0)
	{
		CreateActorInstanceTribalChief(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if(strcmpi(name, "nastycan.obe")==0)
	{
		LoadActor("cactus.obe",tempName);
		CreateActorInstanceNastyCan(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
	if(strcmpi(name, "minerbot.obe")==0)
	{
		LoadActor("bjdynamite.obe",tempName);
		CreateActorInstanceMinerBot(tempInstance, &map.characterInstances, &map.inactiveCharacterInstances);
		done=true;
	}
#endif

	if (!done)
	{
		CreateActorInstanceZookeeper(tempInstance,&map.characterInstances,&map.inactiveCharacterInstances);
		SetStartPosition(tempInstance,tempVector);
	}

	if (done)
	{
		SetStartPosition(tempInstance, tempVector);
		if (box)
		{
			SetupCharacterBox(tempInstance,bankObject,position,orientation,scale);
		}

		// TP: set initial state
		tempInstance->genericAI->characterEnabled = active;

		tempInstance->genericAI->spawnPoint = cageCRC;
	}

#ifndef CONSUMERDEMO
	if (strcmpi(name, "shark.obe")==0)
	{
		if (tempInstance)
		{
			FindPatrolPoints(tempInstance);
		}
	}
#endif

	// PP: return the newly-created enemy ACTORINSTANCE - could be kinda useful
	return tempInstance;
}


/* --------------------------------------------------------------------------------
   Function		: guiSetCheatTick
   Purpose		: add a tick to the cheats menu, to indicate that a particular cheat is active
   Parameters	: cheat whose tick to add (CHEAT_)
   Returns		: 
   Info			: 
*/
void guiSetCheatTick(const ECheat cheat)
{
	int		id;				// PP: FACT: an 'id' is a unit of germoplasm - and a handy word in Scrabble!


	id=cheat+1;// PP: +1: QA cheats have been shifted to the top of the menu

	if(id >= FIRST_DEBUG_CHEAT)
	{
		++id;
	}

	TBGuiMenuItem*	item;

	item=bgFindMenuItemByIndex(cheatMenu, id);

	if(item != NULL)
	{
		bgSetMenuItem(item,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
	}
}


/* --------------------------------------------------------------------------------
   Function		: guiClearCheatTick
   Purpose		: clear a tick from the cheats menu, to indicate that the cheat is not active
   Parameters	: cheat whose tick to clear (CHEAT_)
   Returns		: 
   Info			: 
*/
void guiClearCheatTick(const ECheat cheat)
{
	int		id;				// PP: FACT: an 'id' is a unit of germoplasm - and a handy word in Scrabble!


	id=cheat+1;// PP: +1: QA cheats have been shifted to the top of the menu

	if(id >= FIRST_DEBUG_CHEAT)
	{
		++id;
	}

	bgSetMenuItem(bgFindMenuItemByIndex(cheatMenu, id),NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
}

/* --------------------------------------------------------------------------------
   Function : UpdateGuiItems
   Purpose : updates the state of any gui items before gui is entered
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateGuiItems(void)
{
	// PP: check for null actorInstance
	if(gameStatus.player[0].actorInstance == NULL)
	{
		return;
	}

	// CMD: update state of powerup menu
	if(gameStatus.player[0].actorInstance->characterInfo->currentBurp)
	{
		bgSetMenuItem(burpCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);

		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
	else
	if(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.hiccup)
	{
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);

		bgSetMenuItem(burpCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
	else
	if(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.chilliPepper)
	{
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);

		bgSetMenuItem(burpCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
	else
	if(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.invisible)
	{
		bgSetMenuItem(testTube,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);

		bgSetMenuItem(burpCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
	else
	if(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.bubbleGum)
	{
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,BGUIMENUITEMFLAG_TICKED,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);

		bgSetMenuItem(burpCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
	else
	{
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(burpCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(hiccupCan,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(chilliPepper,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(testTube,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
		bgSetMenuItem(bubblegum,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: SetupConsole
	Purpose		: Setup the onscreen debug print messages
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SetupConsole(void)
{
	bdEnableConsoleWindow(true);
	bdFormatConsoleWindow((-videoMode.xScreen/2)+((videoMode.xScreen-(videoMode.safeWidthPortion*videoMode.xScreen))/2), (videoMode.yScreen/2)-((videoMode.yScreen-(videoMode.safeHeightPortion*videoMode.yScreen))/2), videoMode.yScreen*videoMode.safeHeightPortion);
	bdSetConsoleWindowFont(standardFont, 0.4f, 255, 0, 0);
}


/*  --------------------------------------------------------------------------------
	Function	: FinishLevel
	Purpose		: Complete the level, mark all posters complete and exit level with cutscene
	Parameters	: scene, true to play cutscene immediately
	Returns		: 
	Info		: 
*/

void FinishLevel(int scene, bool complete)
{
	switch(scene)
	{
		// TP: these need special treatment
	case SCENE_ZOOBOSS:
	case SCENE_CITYBOSS:
	case SCENE_WESTBOSS:
	case SCENE_TAZPREBOSS:
	case SCENE_TAZBOSS:
	case SCENE_MUSEUMVEHICLERACE:
	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:
	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:
	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
		break;

	default:
		// TP: mark all posters as complete
		for (int i=1;i<=map.mapStats.numPosters;i++)
		{
			AddWantedPosterToStats(i, gameStatus.player1, scene);
		}

		// TP: end level with cutscene
		if (complete)
		{
			LevelCompleted(gameStatus.player1);
		}
		break;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ForceWin
	Purpose		: Complete current scene with a win scenario
	Parameters	: scene
	Returns		: 
	Info		: 
*/

void ForceWin(int scene)
{
	switch(scene)
	{
#ifndef CONSUMERDEMO
		// TP: these need special treatment
	case SCENE_ZOOBOSS:
		ZooBoss_game.win();
		break;
	case SCENE_CITYBOSS:
		CityBoss_game.cityBossForceEnd(true);
		break;
	case SCENE_WESTBOSS:
		WestBoss_game.win();
		break;
	case SCENE_TAZPREBOSS:
		tazBoss1Game.win();
		break;
	case SCENE_TAZBOSS:
//#ifdef _TAZDEBUG
		allowDebugSkip = TRUE;
//#endif
		break;
	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:
		destructForceEnd(true);
		break;
#endif
	case SCENE_MUSEUMVEHICLERACE:
	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:
		raceForceEnd(true);
		break;

	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
		raceForceEnd(true);
		break;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ForceLoss
	Purpose		: Complete the current scene with a loss scenario
	Parameters	: scene
	Returns		: 
	Info		: 
*/

void ForceLoss(int scene)
{
	switch(scene)
	{
#ifndef CONSUMERDEMO
		// TP: these need special treatment
	case SCENE_ZOOBOSS:
		ZooBoss_game.lose();
		break;
	case SCENE_CITYBOSS:
		CityBoss_game.cityBossForceEnd(false);
		break;
	case SCENE_WESTBOSS:
		WestBoss_game.lose();
		break;
	case SCENE_TAZPREBOSS:
		tazBoss1Game.lose();
		break;
	case SCENE_TAZBOSS:
		break;
	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:
		destructForceEnd(false);
		break;
#endif
	case SCENE_MUSEUMVEHICLERACE:
	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:
		raceForceEnd(false);
		break;
	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
		raceForceEnd(false);
		break;
	}
}


