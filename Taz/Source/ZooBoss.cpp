// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : ZooBoss.cpp
//   Purpose : Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
// Component : Taz
//      Info : // PP: *** THIS WAS MY FIRST ATTEMPT AT A BOSS GAME.  IT'S VERY BADLY WRITTEN - SORRY! ***
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"					// PP: global stuff
#include "main.h"					// PP: main stuff
#include "maths.h"					// PP: maths stuff
#include "LinkedList.h"				// PP: linked list class
#include "actors.h"					// PP: actor stuff
#include "gui.h"					// PP: gui stuff
#include "camera.h"					// PP: camera stuff
#include "files.h"					// PP: provide support functions for actor and texture loading
#include "cameracase.h"				// PP: camera case stuff
#include "KOScam.h"					// PP: "Keep-On-Screen" camera - keeps its target actor instances on-screen at all times
#include "control.h"				// PP: control stuff
#include "parsemap.h"				// PP: map file parser
#include "TextFX.h"					// PP: for RECTANGLE
#include "animation.h"				// PP: provides functions and structures to manage animations
#include "physics.h"				// PP: physics functions
#include "collisioncache.h"			// PP: functions for handling the creation and storage of collision meshes
#include "scene.h"					// PP: scene initialise, setup and draw functions
#include "holysmoke.h"				// PP: smoke effects
#include "status.h"					// PP: pah, actor status?
#include "health.h"					// PP: handles anything to do with a character and its health
#include "sfx.h"					// PP: sound effect rountines
#include "characters.h"				// PP: provides character response code
#include "Shockwave.h"				// PP: Shockwave effect
#include "Icon.h"					// PP: Icon effects used by the front end
#include "BrickDisplay.h"			// PP: 'Brick Wall' score display for the Zoo boss game
#include "clouds.h"					// PP: functions for creating and updating collectibles
#include "lights.h"					// PP: lights
#include "Glare.h"					// PP: Light glare effects
#include "Flies.h"					// PP: Midges and the like
#include "util.h"					// PP: utility functions
#include "quakecam.h"				// PP: shakey cameras
#include "decals.h"					// PP: Functions for drawing decals using the shadow information
#include "display.h"				// PP: display stuff
#include "player.h"					// PP: player stuff, for taz's shadowmap radius
#include "pause.h"					// PP: pause menus
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "playerstats.h"			// TP: Level complete function and stat displays
#include "Boss.h"					// PP: General boss game stuff
#include "ZooBoss.h"				// PP: temp test
#include "shadow.h"					// PP: shadows
#include "PageItemFX.h"				// PP: Special effects for page items
#include "PongDust.h"				// PP: Cartoon dust effect, for Elephant Pong
#include "cutscene.h"				// PP: provides functions to initialise,play and free cutscenes

/*#ifndef PHIL
#error "PP: OOPS, I didn't mean to check this in.  Please slap me."
#endif// PP: ndef PHIL*/
/* PP: 

// PP: TODO: GET SHADOWS ON EVERYONE IN THE INTRO & OUTRO CUTSCENES

  INTRO:

	cs05_elephant.obe			motion_stompy
	cs05_gossamer.obe			motion_gossamer
	cs05_shedevil.obe			motion_shedevil
	cs05_tazdevil.obe			motion_taz?
	cs05_yosamite.obe			motion_yosamite

  OUTRO:

	cs06_elephant.obe			motion_stompy
	cs06_gossamer.obe			motion_gossamer
	cs06_shedevil.obe			motion_shedevil
	cs06_tazdevil.obe			motion_taz?
	cs06_spindevil.obe			motion_spin????
	cs06_yosamitesam.obe		motion_yosamite
	cs06_truck-shedevil.obe		

*/

// PP: I'm lazy
#define zbsam		(ZooBoss_game.sam)
#define zbball		(ZooBoss_game.ball)

// PP: Y position of the court floor
#define ZB_COURT_HEIGHT				0.0f// PP: METERTOUNIT(0.05f)

// PP: TEMP TEST!
float		g_circleRadius;
TBVector	g_circleOffset;

// PP: temp for tweaky
float		runDustFreq=60;//30.0f;

// PP: MSBODGE
static bool	g_forceResult=false;

// PP: MSBODGE
#define	ZB_TEMP_OUTRO_DELAY		1.6f// PP: 4.0f

//TBActorInstance		ZBshadowZMask;

// PP: use proper shadows for the Zoo Boss
#define ZB_SHADOWMAP

// PP: use cartoony dust for the Zoo Boss
#define ZB_PONGDUST

// PP: use a spinoff effect after each goal (disabled in stereoscopic modes, to save hassle)
#define ZB_SPINOFF

// PP: define ZB_JUMP to allow the players to jump.
#define ZB_JUMP

// PP: define ZB_CONTAIN_CAMERA to force stop the camera going out of the area that we want to keep it in (not ready yet)
#define ZB_CONTAIN_CAMERA

// PP: define ZB_BITE to allow the players to bite.  Now why would we want to go and do a thing like that eh?
// PP: it seems you can still slide around which biting - that would need fixing
//#define ZB_BITE

//#define ZB_STATIC_BALL// PP: COMPLETELY STOP THE ELEPHANT FROM MOVING (FOR TEMPORARY DEBUG ONLY)

#ifndef INFOGRAMES// PP: don't want debug lines on a disk
#define ZOOBOSS_DEBUG// PP: YES! I WAN'T DEBUG FACILITATEYS, PLEASE!

// PP: REMOUT: no the noo		#define ZBBAT_HEADTRACK// PP: not quite ready yet - this should not have been a complicated thing.
#endif// PP: def INFOGRAMES

#define ZB_SUN_GLARE_COLOUR						COLOUR(255, 220, 100, 50)// PP: COLOUR(255, 200, 80, 50)					// PP: colour of sun glare
#define ZB_SUN_GLARE_ANGLE_RANGE				0.6f
												
#define ZB_DURATION_STANDARDMESSAGE				1.5f
												
static float ZooBoss_bookScale					=1.0f;

// PP: text style used for the intro Latin ID gag
TEXTSTYLE										TS_ident;
												
ZOOBOSSGAME										ZooBoss_game;

// PP: initialise static elephant footprint texture
TBTexture*										ZBBALL::footprintTexture=NULL;
												
static TEXTBOX*									ZooBoss_debugBox;
static BOOK*									ZooBoss_messageBook;
// PP: REMOUT: not used	static uint32*									gpFlags;
												
// PP: Colours of the rings in Gossamer's shockwaves: blue first, then subdued green, then red
// PP: The effect is meant to look a bit like colour dispersion see?  No, I don't know why.  Go away.
static COLOUR									ZooBoss_shockwaveColours[ZBSHOCKWAVE_NUMRINGS]={COLOUR(0,0,128), COLOUR(0,32,0), COLOUR(128,0,0)};
												
static VEC										ZooBoss_sunDir=VEC(0.1f,0,-1).normalise2();/*.rotate(0.096000f, 4.710000f);*/// PP: {0.0f, 0.0f, -1.0f, 0.0f};// PP: was 1.0f; Babel musta changed
static VEC										ZooBoss_shadowAng=VEC(0.624695f, 0.000000f, -0.780869f);

#define SHADOWMAP_RADIUS_GOSSAMER				METERTOUNIT(8.5f)// PP: METERTOUNIT(6)	// PP: radius of Gossamer's shadow map
#define SHADOWMAP_RADIUS_ELEPHANT				METERTOUNIT(17)	// PP: radius of the Elephant's shadow map

#define ZBBATAI_MAX_BALL_PATH_LINE_SEGMENTS		500		// PP: maximum number of path line segments that will be calculated by the bat AI in order to get an idea where the ball's going to go

// PP: initialise some static stuff for ZOOBOSSGAME
TBActorInstance*								ZOOBOSSGAME::shadowZMask=NULL;

// PP: ******** Bat AI debug stuff **********

bool											ZooBoss_debug=false;

static int										numPathVerts=0;
static TBPrimVertex								pathVerts[BOSS_NUM_PLAYERS*(ZBBATAI_MAX_BALL_PATH_LINE_SEGMENTS+1)];

#define ZBBALL_TUSK_ANGLE_RANGE					0.6f			// PP: angle range within which a bat will be prodded by elephant's tusks



/*	--------------------------------------------------------------------------------
	Function 	: ZooBoss_introIDpauseUpdateFunc
	Purpose 	: update function for the custom pause state 
	Parameters 	: long
	Returns 	: 
	Info 		: must match the PauseUpdateFunc typedef (main.h).
*/
void ZooBoss_introIDpauseUpdateFunc(const int32 parameter)
{
	ZooBoss_game.update();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														ZBSAM methods

				Yosemite Sam, observing the match from his lookout tower and no doubt shouting a lot and getting angry

															@zbsam
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: sample names for sam's phrases
char* ZBSAM::speechSamples[ZBSAM_NUM_PHRASES]=
{
	"samzoobossmoan_1.wav",			// PP: I said SCARE the elephant, not invite him to dinner!
	"samzoobossmoan_2.wav",			// PP: Come on! Get that critter!
									
	"samzoobosstaunt_1.wav",		// PP: You're a-going DOWN, devil!
	"samzoobosslose_1.wav",			// PP: You're as bad as that pesky duck...
									
	"samzoobossgen_1.wav"			// PP: That elephant sure can pong!
};


// PP: anim names for sam's anims
char* ZBSAM::anims[ZBSAM_NUM_ANIMS]=
{
	"idle1",						// PP: breathe

	"destroybox1",					// PP: slap face, shake head
	"destroybox2",					// PP: jump in air kicking & punching
	"destroybox3",					// PP: twang moustache
	"destroysupercrate1",			// PP: jump on hat

	"injuretaz1",					// PP: belly laugh
	"tazdie",						// PP: throw hat
};


/*	--------------------------------------------------------------------------------
	Function 	: ZBSAM::init
	Purpose 	: initialisation at beginning of boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define ZBSAM_MODEL		"yosemite.obe"			// PP: sam's model
#define ZBSAM_POS		VEC(153,2187,-2870)		// PP: sam's position

void ZBSAM::init(void)
{
	ACTORINSTANCEUSER::init(ZBSAM_MODEL, ZBSAM_POS, PI);

	// PP: not found cutscene bone yet
	this->cutsceneNode=NULL;

	this->playAnim(ZBSAMANIM_IDLE, true);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBSAM::initPreCutscene
	Purpose 	: initialisation before the intro cutscene
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBSAM::initPreCutscene(void)
{
	// PP: not found cutscene sam yet
	this->cutsceneActorInstance=NULL;

	// PP: not found cutscene bone yet
	this->cutsceneNode=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBSAM::update
	Purpose 	: per-frame update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBSAM::update(void)
{
	// PP: Y-rotate to always face the elephant
	this->YrotateToFace(zbball);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBSAM::win
	Purpose 	: react to gossamer scoring a goal
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBSAM::win(void)
{
	this->say((bmRand()&1)?ZBSAMSPEECH_TAUNT1:ZBSAMSPEECH_TAUNT2);

	this->playAnim((bmRand()&1)?ZBSAMANIM_WIN1:ZBSAMANIM_WIN2);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBSAM::lose
	Purpose 	: react to gossamer losing a goal
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBSAM::lose(void)
{
	this->say((bmRand()&1)?ZBSAMSPEECH_ANGRY1:ZBSAMSPEECH_ANGRY2);

	this->playAnim((bmRand()&1)?ZBSAMANIM_WIN1:ZBSAMANIM_WIN2);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													ZOOBOSSGAME methods

					This is the BOSSGAME type for the Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown

														@zoobossgame
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: fov used for bat closeups
#define ZBCAM_GOAL_CLOSEUP_FOV			RAD(50.0f)


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::draw2
	Purpose 	: draw the zoo boss game alpha effects
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::draw2
*/
void ZOOBOSSGAME::draw2(void)
{
	BOSSGAME::draw2();

	// PP: draw court rects	
/*	for(int i=0; i<BOSS_NUM_PLAYERS; i++)
	{
		//this->bats[i].court.draw(RECTANGLE_DEFAULT_DRAW_COLOUR, RECTANGLE_DEFAULT_DRAW_FLAGS, X, Z, ZB_COURT_HEIGHT);
		this->bats[i].draw();
	}*/

	// PP: draw books and break out if this is not a player-specific screen
	if(gameStatus.multiplayer.currentScreen == -1)
	{
		return;
	}
	else
	{
		this->bats[gameStatus.multiplayer.currentScreen].draw();
	}

	this->bats[1].draw();
	
	// PP: TEMP TEST - draw gos's coll sphere
/*	TBVector	centrePos;
	bmVectorAdd(centrePos, this->bats[1].pos(), g_circleOffset);
	utilDrawSphere(centrePos, g_circleRadius, COLOUR(0, 255, 0, 255));*/

	// PP: TEMP TEST
	/*
	bdSetIdentityObjectMatrix();
*/

#ifdef ZB_PONGDUST
		if(!PLAYING_OUTRO_CUTSCENE)// PP: cutscene (hence cutscene spin) get drawn really late so I gotta do this elsewhere
		{
			// PP: draw dust in game and in intro cutscene
			PongDust_draw();
		}
#endif// PP: def ZB_PONGDUST

	if(this->initialised)
	{
		if(ZooBoss_debug)
		{
			if(numPathVerts > 0)
			{
				// PP: draw the ball's path, for debugging bat ai ...

				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
				bdSetTexture(0,NULL);

				ASSERT(numPathVerts);
				bdDrawPrimitive(BDPRIMTYPE_LINELIST,pathVerts,numPathVerts,BVERTTYPE_SINGLE);
			}
		}

		// PP: work out if the elephant is blocking the sun...

		TBCollisionSphere	sphere;
		TBCollisionRay		ray;

		bmVectorCopy(ray.start, gameStatus.viewPoint);
		bmVectorCopy(ray.unitVector, ZooBoss_sunDir.v);

		// PP: length of ray used on the Zoo Boss to check for the sun being blocked (to determine whether or not to draw sun glare)
#define ZB_SUN_BLOCKAGE_RAY_LENGTH		METERTOUNIT(100)

		ray.length=ZB_SUN_BLOCKAGE_RAY_LENGTH;

		bmVectorCopy(sphere.position, this->ball.pos());

#define ZBBALL_SUN_BLOCKAGE_RADIUS		METERTOUNIT(2.75f)
#define ZBBALL_SUN_BLOCKAGE_HEIGHT		METERTOUNIT(2.5f)

		sphere.radius=ZBBALL_SUN_BLOCKAGE_RADIUS;

		sphere.position[Y]+=ZBBALL_SUN_BLOCKAGE_HEIGHT;

		// PP: TEMP TEST!!!!
		//utilDrawSphere(sphere.position, sphere.radius, WHITE);

		if(!bmIntersectRaySphere_Unit(&ray, &sphere))
		{
			float rtn=Glare_drawDirectional(ZooBoss_sunDir.v, ZB_SUN_GLARE_COLOUR, ZB_SUN_GLARE_ANGLE_RANGE);
		}
	}
	else
	{
		// PP: have Yosemite Sam's bit fat hat block the sun instead

		// PP: find and keep a pointer to the motion bone of the cutscene sam instance...

		if(this->sam.cutsceneActorInstance == NULL)
		{
			this->sam.cutsceneActorInstance= FindActorInstanceInMap("cs05_yosamite.obe", 0, &map);
		}

		if(this->sam.cutsceneActorInstance != NULL)
		{
			if(this->sam.cutsceneNode == NULL)
			{
				this->sam.cutsceneNode=baFindNode(this->sam.cutsceneActorInstance->actorInstance.rootNodeInstance, "motion_yosamite");
			}

			TBCollisionSphere	sphere;
			TBCollisionRay		ray;

			bmVectorCopy(ray.start, gameStatus.viewPoint);
			bmVectorCopy(ray.unitVector, ZooBoss_sunDir.v);
			ray.length=METERTOUNIT(100);

			baGetNodesWorldPosition(&this->sam.cutsceneActorInstance->actorInstance, this->sam.cutsceneNode, sphere.position);

#define ZBSAM_SUN_BLOCKAGE_RADIUS		METERTOUNIT(0.75f)
#define ZBSAM_SUN_BLOCKAGE_HEIGHT		METERTOUNIT(0.8f)

			sphere.radius=ZBSAM_SUN_BLOCKAGE_RADIUS;

			sphere.position[Y]+=ZBSAM_SUN_BLOCKAGE_HEIGHT;

			// PP: TEMP TEST!!!
			//utilDrawSphere(sphere.position, sphere.radius, WHITE);

			if(!bmIntersectRaySphere_Unit(&ray, &sphere))
			{
				float rtn=Glare_drawDirectional(ZooBoss_sunDir.v, ZB_SUN_GLARE_COLOUR, ZB_SUN_GLARE_ANGLE_RANGE);
			}
		}
	}

	// PP: draw tower box
/*	{
		SETPROJECTION(3D);
		bdSetIdentityObjectMatrix();

		BOUNDINGBOX		tempBox;

		tempBox=this->ball.towerBox;

		tempBox.draw();

		utilDrawLine(this->towerRay.start, this->towerRay.start+VEC(this->towerRay.unitVector), RED);
	}*/

	// PP: draw the ball
// PP: tst	this->ball.draw();
//	this->ball.pos()[X]= -METERTOUNIT(100);

/*
	// PP: TEMP TEST - show sun dir and cam dir

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);

#define TEMPHEIGHT		METERTOUNIT(3)
#define TEMPLENGTH		METERTOUNIT(3)

	// PP: camera dir based on viewAng
	utilDrawLine( VEC(0, ZB_COURT_HEIGHT+TEMPHEIGHT), (VEC(0, ZB_COURT_HEIGHT+TEMPHEIGHT)+(VEC(0, 0, TEMPLENGTH).rotate(gameStatus.viewAng))), CYAN);

	// PP: sun dir
	utilDrawLine( VEC(0, ZB_COURT_HEIGHT+TEMPHEIGHT), (VEC(0, ZB_COURT_HEIGHT+TEMPHEIGHT)+(ZooBoss_sunDir*TEMPLENGTH)), RED);
*/
	//ball.contactCourt.draw(CYAN, OUTLINE, X, Z, ZB_COURT_HEIGHT);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::initScoreBook
	Purpose 	: initialise the book for overlays such as score readouts
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initScoreBook
*/
void ZOOBOSSGAME::initScoreBook(void)
{
	PAGE*			page;
	ICON*			icon;
	TEXTBOX*		textBox;

	BOSSGAME::initScoreBook();


	// PP: set the rectangle for the score book (whole screen, but only the top is used)
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		this->scoreBook.setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-310, 310, -220, 220));

#define ZB_SCORETEXT_FONTSIZE		1.6f		// PP: temp

	// PP: Taz's icon
	page=this->scoreBook.addPage(icon=new ICON("extras\\tazicon.bmp", "TAZICON"), NULL, "SCOREPAGE");
	//page->setName("2PSCOREPAGE");
	icon->setXPadding(false);
	icon->tilt=false;
	icon->setYAlign(PIFLAG_YALIGN_TOP);

	// PP: todo: Taz's score display
	// PP: TEMP: textbox to print taz's score into
	page->insertItem(textBox=new TEXTBOX(), false);
	textBox->setXAlign(PIFLAG_XALIGN_LEFT);
	textBox->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setFontSize(ZB_SCORETEXT_FONTSIZE);
	textBox->setColour(COLOUR(0,255,0), COLOUR(0,20,0));
	textBox->setName("TAZSCORE");
	textBox->clear();


	// PP: debug text box
/*	page->insertItem(ZooBoss_debugBox=new TEXTBOX((EStringIndex)(-1), COLOUR(255, 0, 0), COLOUR(0, 0, 255), TBFLAG_DEFAULTS), false);
	ZooBoss_debugBox->setDrawScale(0.6f);
	ZooBoss_debugBox->setAlignScale(0.0f);
	ZooBoss_debugBox->setYAlign(PIFLAG_YALIGN_TOP);
	ZooBoss_debugBox->setWrap(false);
	ZooBoss_debugBox->clear();*/

	// PP: Gossamer's score display...

	// PP: TEMP: textbox to print Gossamer's score into
	textBox=new TEXTBOX();
	textBox->setXAlign(PIFLAG_XALIGN_RIGHT);
	textBox->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setFontSize(ZB_SCORETEXT_FONTSIZE);
	textBox->setColour(COLOUR(255,0,0), COLOUR(20,0,0));
	textBox->setName("GOSSAMERSCORE");
	textBox->clear();

	page->insertItem(textBox, this->twoPlayer);

	// PP: Gossamer's icon
	page->insertItem(icon=new ICON("extras\\gossamericon.bmp", "GOSSAMERICON"), false);
	icon->setXAlign(PIFLAG_XALIGN_RIGHT);
	icon->setXPadding(false);
	icon->tilt=false;
	icon->setYAlign(PIFLAG_YALIGN_TOP);

	//page->setScale(ZooBoss_bookScale);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::initMessageBook
	Purpose 	: initialise the book for messages such as "GET READY..." or "GOAL!"
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initMessageBook
*/
#define ZB_MESSAGE_FONT_SIZE		3.0f			// PP: font size for messages such as "GET READY..." or "GOAL!"

void ZOOBOSSGAME::initMessageBook(void)
{
	PAGE*			page;
	TEXTBOX*		textBox;

	BOSSGAME::initMessageBook();

	// PP: TODO: loads more to add

	// PP: set the rectangle for the message book
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		this->messageBook.setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-310, 310, -220, -50));

	// PP: set up the text style used for the Latin ID gag
	{
		TS_ident.italic=false;
		TS_ident.vGrad=false;
		TS_ident.colour1=COLOUR(128,128,128,128);
	}

	// PP: elephant id page...
	page=this->messageBook.addPage("ELEPHANTID");
	textBox=page->insertFreeItem(new TEXTBOX(STR_ZOOBOSS_ELEPHANTID), RECTANGLE(-0.5f, 0.5f, -0.5f, -0.1f))->textbox();
	textBox->setWrap(true);
	textBox->setStyle(TS_ident);

	// PP: no effects on the ID page
	page->setEffectChooser(&basePIEChooser);

	// PP: 'get ready' page
	page=this->messageBook.addPage(STR_COUNTDOWN_GETREADY, NULL, "PREPAREZVOUS");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: 'go' page
	page=this->messageBook.addPage(STR_COUNTDOWN_GO, NULL, "GO");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: 'goal' page
	page=this->messageBook.addPage(STR_ZOOBOSS_GOAL, NULL, "GOAL");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: 'oops' page
	page=this->messageBook.addPage(STR_ZOOBOSS_LOSEPOINT, NULL, "LOSEPOINT");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: win page
	page=this->messageBook.addPage(STR_ZOOBOSS_WIN, NULL, "WIN");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: lose page
	page=this->messageBook.addPage(STR_ZOOBOSS_LOSE, NULL, "LOSE");
	((TEXTBOX*)(page->getLastItem()))->setFontSize(ZB_MESSAGE_FONT_SIZE);

	// PP: 2-player result screen...

	page=this->messageBook.addPage("2P RESULT");
	page->insertItem(STR_MP_WINNER)
		->setFontSize(2.75f)
		->setWrap(true)
		->setYAlign(PIFLAG_YALIGN_TOP);

	// PP: for consistency, use the pie chooser used in the races and bonus games
	this->messageBook.setEffectChooser(&racePIEChooser);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::initInstructionBook
	Purpose 	: initialise the book for instructions such as "press X to start"
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initInstructionBook
*/
#define ZB_INSTRUCTION_FONT_SIZE		1.0f			// PP: font size for instructions

void ZOOBOSSGAME::initInstructionBook(void)
{
	BOSSGAME::initInstructionBook();

	// PP: moved to bat::init
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::win
	Purpose 	: win the boss game
	Parameters 	: (opt/0)player to win
	Returns 	: 
	Info 		: this overrides BOSSGAME::win
*/
void ZOOBOSSGAME::win(const int weiner)
{
	g_forceResult=true;

	this->winner=weiner;
	this->loser=weiner^1;

	while(this->bats[this->winner].getScore()<ZooBoss_game.getMaxScore()-1)
	{
		this->bats[this->winner].increaseScore();
	}

	this->ball.koikWall=this->bats[this->loser].wall;

	this->ball.setState(ZBBALLSTATE_GOAL);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::lose
	Purpose 	: lose the boss game
	Parameters 	: (opt/0)player to lose
	Returns 	: 
	Info 		: this overrides BOSSGAME::lose
*/
void ZOOBOSSGAME::lose(const int luzer)
{
	g_forceResult=true;

	this->loser=luzer;
	this->winner=luzer^1;

	while(this->bats[this->winner].getScore()<ZooBoss_game.getMaxScore()-1)
	{
		this->bats[this->winner].increaseScore();
	}

	this->ball.koikWall=this->bats[this->loser].wall;

	this->ball.setState(ZBBALLSTATE_GOAL);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::setState
	Purpose 	: switch the game into a new state
	Parameters 	: the state into which to switch
	Returns 	: 
	Info 		: this overrides BOSSGAME::setState
*/
void ZOOBOSSGAME::setState(const int newState)
{
	switch(newState)
	{
	case BGSTATE_INTRO:
		{
			//StopCameraCase(&camera[0], this->debugCameraCase);
			//StartCameraCase(this->cameraCase, &camera[0]);

			this->setCamera(ZBCAM_INTROHOOT);

			SetCameraSmooth(&camera[0], 1.0f, 1.0f);

			// PP: REMOUT: now done in cutscene		this->ball.setState(ZBBALLSTATE_INTRODROP1);
			this->ball.setState(ZBBALL_INITIAL_STATE);
		}
		break;

	case BGSTATE_PLAY:
		{
			// PP: open the score book on the appropriate page
			this->scoreBook.open(/*this->twoPlayer?"2PSCOREPAGE":"1PSCOREPAGE"*/);
		}
		break;

	case BGSTATE_OUTRO:
		{
			// PP: close scare prompts
			for(int i=0; i<BOSS_NUM_PLAYERS; i++)
			{
				this->bats[i].book.close();
			}

			// PP: close score book
			this->scoreBook.close();

			if(this->twoPlayer)
			{
				// PP: VERY DODGY: need this to disable pause mode
				gameStatus.controlSuspend=true;

				// PP: update winner number
				this->messageBook.getPage("2P RESULT")->getTextbox(STR_MP_WINNER)->setText(STR_MP_WINNER, this->winner+1);

				// PP: open two-player result screen
				this->messageBook.open("2P RESULT");

				this->setCamera(ZBCAM_2PLAYER_OUTRO);
			}
			else
			{
				this->setState(BGSTATE_FINISHED);
			}
		}
		break;

	case BGSTATE_FINISHED:
		{
			// PP: TEMP

			//StopCameraCase(&camera[0], this->debugCameraCase);
			//StartCameraCase(this->cameraCase, &camera[0]);

			// PP: close the book that holds the "SCARE" prompt
		// PP: REMOUT: already done in setstate(outro)	this->instructionBook.close();

			if(this->twoPlayer)
			{
				bkGenerateEvent("scenechange","_frontend");
			}
			else
			{
				// PP: TEMP TEST
				delay = ZB_TEMP_OUTRO_DELAY;

				if(this->winner == 0)
				{
					this->messageBook.open("WIN");
					// PP: REMOUT: TEMP TEST		delay = ZB_TEMP_OUTRO_DELAY;
					setState(BGSTATE_CUTSCENE);
				}
				else
				{
					this->messageBook.open("LOSE");
					this->messageBook.close(2.2f);


					for(int i=0; i<BOSS_NUM_PLAYERS; i++)
					{
						this->bats[i].book.close();
					}

					this->scoreBook.close();

					ProcessTazDeathAtEndOfBossGame(ZOOBOSSGAME::resetCallback, NULL, NULL, ZOOBOSSGAME::fadeDownCallback);
				}
			}
		}
		break;

	case BGSTATE_CUTSCENE:
		{
			// PP: shutdown a few things...

			// PP: SAM
		/*	this->sam.shutdown();

			// PP: TAZ & GOSSAMER
			for(int i=0; i<BOSS_NUM_PLAYERS; i++)
			{
				this->bats[i].shutdown();
			}*/
		}
/* PP: REMOUT: not doing anything!		if ((delay-=fTime)<=0.0f)
		{
			// PP: hmm, this function scares me a bit - it should be used carefully
			BookList_closeAll();

			this->messageBook.close(0.0f, true);
			if(this->winner == 0)
			{
				LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
			}
			else
			{
				bkGenerateEvent("scenechange","_zoohub");
			}

			// TP: Added this to stop repeated levelcomplete calls being made
			setState(BGSTATE_NONE);
		}*/
		break;
	}

	BOSSGAME::setState(newState);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::initpreCutscene
	Purpose 	: initialisation for the Zoo Boss game, before the intro cutscene has run
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initPreCutscene
*/
void ZOOBOSSGAME::initPreCutscene(void)
{
	BOSSGAME::initPreCutscene();

	ZooBoss_debug=false;

	// PP: get hold of elephant's eyes (for cutscene)
	this->ball.eyeTexture=bkLoadTexture(NULL, ZBBALL_CUTSCENE_EYE_TEXTURE, 0);

	// PP: TODO: set up shadows, dust, environmental things that'll be needed in the cutscene as well as the game

#ifdef ZB_PONGDUST
	// PP: Setup dust
	PongDust_sceneInit();
#endif// PP: def ZB_PONGDUST

	// PP: make the shadow zmask actorinstance...
	{
		TBActor*			actor=NULL;

	// PP: moveme?
#define ZB_SHADOW_ZMASK_ACTOR		"extras\\shadowzmask.obe"

		LoadActor(ZB_SHADOW_ZMASK_ACTOR,map.mapName,0);

		actor=FindActorInActorList(ZB_SHADOW_ZMASK_ACTOR);

		ZOOBOSSGAME::shadowZMask=baInitActorInstance(NULL, actor, 0);

		// PP: must prep before first draw
		baPrepActorInstance(ZOOBOSSGAME::shadowZMask,BACTORPREPFLAG_PREPNODES);
	}

#if(BPLATFORM != PS2)
	if(CheatActive(CHEAT_AIRBRUSH))
	{
		SetLightRamp(LIGHTS_DEFAULT_RAMP);
	}
	else
	{
		// PP: before creating any actor instances (and enabling cell shading), set the ramp we want for this scene
		SetLightRamp(RAMP_SUNSET);
	}
#endif// PP: platform

#ifdef ZB_SHADOWMAP
	// PP: bring on the decent shadows!
	VFX_activateEffect(VFX_SHADOWMAP);
#endif// PP: def ZB_SHADOWMAP

	// PP: init sam
	this->sam.initPreCutscene();

	// PP: init ball
	this->ball.initPreCutscene();
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::init
	Purpose 	: initialise the Zoo Boss game
	Parameters 	: rectangle for playing area
	Returns 	: 
	Info 		: this overrides BOSSGAME::init
*/

#define ZB_GAME_SPEED		1.2f		// PP: global speed of the Zoo Boss game

/* PP: TODO: TRY THIS LIGHT

pos 123.167480, 299.992188, -766.955933
ang 0.096000, 4.710000, 0.000000
col 255, 255, 156

*/

void ZOOBOSSGAME::init(const RECTANGLE &courtIn)
{
	TBVector		pos;
	RECTANGLE		rect;
	int				p;


	BOSSGAME::init();

	g_forceResult=false;

	// PP: one-player score is zero
	this->score=0;

	// PP: update the text in the score display
// PP: 	this->updateScoreDisplay();

	// PP: SET ZOOBOSS GAME SPEED
	gameStatus.gameSpeed=ZB_GAME_SPEED;

	// PP: SET UP SOUND
	SetHearingRange(ZB_HEARING_RANGE);
	
	// PP: keep a global pointer to the message book - the ball needs to use it for a start
	ZooBoss_messageBook= &this->messageBook;

	// PP: set the playing area
	this->court=courtIn;
/* PP: REMOUT: precutscene 
	// PP: REMOUT: MILESTONE BODGE: NOT SURE WHY, BUT AT THE MOMENT THIS IS MAKING EVERYTHING BLACK ON PS2 - FIND OUT WHY!!!!!
//#if(BPLATFORM != PS2)
	if(CheatActive(CHEAT_AIRBRUSH))
	{
		SetLightRamp(LIGHTS_DEFAULT_RAMP);
	}
	else
	{
		// PP: before creating any actor instances (and enabling cell shading), set the ramp we want for this scene
		SetLightRamp(RAMP_SUNSET);
	}
//#endif// PP: platform

#ifdef ZB_SHADOWMAP
	// PP: bring on the decent shadows!
	VFX_activateEffect(VFX_SHADOWMAP);
#endif// PP: def ZB_SHADOWMAP
*/
	// PP: initialise 'bat' player1

	// PP: portion of total court length available to a bat
#define ZBBAT_COURT_PORTION		0.11f//0.143f// PP: 0.192f

	rect.init(this->court.right-(this->court.width()*ZBBAT_COURT_PORTION),
				this->court.right,
				this->court.bottom,
				this->court.top);

	this->bats[0].init(PI+HALFPI, rect, &this->ball.court.right, ZBBATTYPE_TAZ,
						(TEXTBOX*)(this->scoreBook.getItem("TAZSCORE")),
						&gameStatus.player[0]);

	// PP: create the opponent 'bat' Gossamer
	PLAYER*			gossPlayer;

	if(this->twoPlayer)
	{
		gossPlayer=&(gameStatus.player[1]);
	}
	else
	{
		gossPlayer=NULL;
	}

	rect.init(this->court.left,
				this->court.left+(this->court.width()*ZBBAT_COURT_PORTION),
				this->court.bottom,
				this->court.top);

	this->bats[1].init(HALFPI, rect, &this->ball.court.left, ZBBATTYPE_GOSSAMER,
						(TEXTBOX*)(this->scoreBook.getItem("GOSSAMERSCORE")),
						gossPlayer);

	// PP: create sam, watching from his tower
	this->sam.init();

	// PP: create the 'ball' Elephant
	SETVECTOR(pos, 0,METERTOUNIT(0.05f),0,1);

	if(bmRand()&1)
	{
		this->ball.init(pos, HALFPI, this->court);
	}
	else
	{
		this->ball.init(pos, PI+HALFPI, this->court);
	}

	// PP: TEMP - tell bats to look at ball
	this->bats[0].lookAt(&this->ball);
	this->bats[1].lookAt(&this->ball);

	// PP: set up the debug keep-on-screen camera~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	TBVector	cameraAngle;
	SETVECTOR(cameraAngle, 5.22f, 5.70f, 0.0f, 1.0f);

	this->debugCameraCase = CreateCameraCase("ZooBoss cameraCase", CAMERACASETYPE_KEEPONSCREEN);	// PP: create the camera case
	CameraCaseKeepOnScreen(this->debugCameraCase, cameraAngle);
	this->debugCameraCase->KOScamData->addTarget(&(this->bats[0].actorInstance->actorInstance));
	this->debugCameraCase->KOScamData->addTarget(&(this->bats[1].actorInstance->actorInstance));
	this->debugCameraCase->KOScamData->addTarget(&(this->ball.actorInstance->actorInstance));
	
	// PP: set up the normal tennis cam~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
//	this->cameraCase = CreateCameraCase("ZooBoss cameraCase", CAMERACASETYPE_ROTATEAROUND);
//	CameraCaseRotateAround(this->cameraCase, "elephant", METERTOUNIT(5.0f), RAD(1.0f), CAMERACASE_FIXEDDISTANCEFROMTAZ);


	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	// PP: add some midges in a suitable place
	SETVECTOR(pos, METERTOUNIT(5.5f), METERTOUNIT(2.0f), METERTOUNIT(-9), 1.0f);// PP: -7
	Flies_addGroup(pos, COLOUR(250, 250, 240, 110), 110);

	// PP: TEMP?: set up the light direction
	// PP: UNREMOUT: TEMP TEST
	// PP: UpdateCartoonLight(ZooBoss_sunDir, COLOUR(127, 100, 64));				// PP: this is ok - InitCartoonLight has been called earlier on in InitialiseScene

	{
#define ZB_SHADOW_CAST_RANGE		1.0f

		shadowLights.init();
		addShadowLight(	VEC(-847.266f, 5721.38f, -6478.04f),// PP: test
						SHADOWLIGHTTYPE_DIRECTIONAL,
						ZooBoss_shadowAng,
						COLOUR(149,125,76),
						0.452478f,
						SHADOWLIGHT_INFINITE_RANGE,
						SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_LANDSCAPE_ONLY),
						SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_LANDSCAPE_ONLY),
						ZB_SHADOW_CAST_RANGE,
						SHADOWLIGHT_DEFAULT_SOFTNESS,
						SHADOWBLEND_SUBTRACTIVE);
	}

	// PP: Start off in the intro state
	this->setState(BGSTATE_INTRO);

	for(p=0; p<gameStatus.multiplayer.numPlayers; p++)
	{
		// PP: set lookat offset for camera
		SETVECTOR(pos, 0.0f, METERTOUNIT(2.0f), 0.0f, 1.0f);
		SetCameraLookAtActorInstanceOffset(&camera[p], pos);
	}

	// PP: NEW
	numPathVerts=0;
}



/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::respondToMessage
	Purpose 	: handle a message from the ball or a bat
	Parameters 	: pointer to actor instance, pointer to message string
	Returns 	: true if the message was recognised and handled, else false
	Info 		: 
*/
bool ZOOBOSSGAME::respondToMessage(const ACTORINSTANCE* const actorInstance, const char* const message)
{
	ZBBAT*	bat=NULL;			// PP: pointer to the bat with the specified actor instance, where applicable

	if(stricmp(message, "pausestart") == 0)
	{
		// PP: start elephant ID gag
		this->messageBook.open("ELEPHANTID", true);
		
		return true;
	}
	else if(stricmp(message, "pauseend") == 0)
	{
		// PP: end elephant ID gag
		this->messageBook.close(0.0f, true);

		return true;
	}
	else if(this->ball.respondToMessage(actorInstance, message))
	{
		return true;
	}
	else
	{
		for(int b=0; b<BOSS_NUM_PLAYERS; b++)
		{
			if(actorInstance == this->bats[b].actorInstance)
			{
				bat= &(this->bats[b]);

				if(bat->respondToMessage(message))
				{
					return true;
				}

				break;
			}
		}

		if(bat == NULL)
		{
			bkPrintf("ZOOBOSSGAME::respondToMessage: actorInstance %X not found!!!!\n");
		}

		return false;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::cameraLookAtBat
	Purpose 	: point the camera at a bat
	Parameters 	: index of bat at which to look
	Returns 	: 
	Info 		: 
*/
void ZOOBOSSGAME::cameraLookAtBat(const int index)
{
	TBVector		camPos;
	TBVector		lookat;
	TBVector		tempVec;
	float			yOffset;
	float			distFromBat;
	CAMERAINFO*		cam;
	SCREEN*			screen;

	if(gameStatus.multiplayer.numScreens == 2)
	{
		// PP: use bat's own camera
		cam= &camera[index];

		screen= &videoMode.screens[index];
	}
	else
	{
		// PP: only one camera
		cam= &camera[0];

		screen= &videoMode.screens[0];
	}

	// PP: ~~~~
	// PP: no actor-instance following
	DisableCameraLookAtFollowActorInstance(camera);
	SetCameraPosFollowActorInstance(camera,NULL,NULL);

	// PP: none of this
	SetFixedDistanceToLookAt(camera, 0.0f);

	// PP: only the flags we need
	camera->flags=CAMERA_NOCOLLISION|CAMERA_RUMBLEFADE;
	
	// PP: don't want lerping
	SetCameraSmooth(camera, 1.0f, 1.0f);
	// PP: ~~~~

	// PP: low fov to stop character looking enormous
	setFOV(ZBCAM_GOAL_CLOSEUP_FOV, screen);

	if(this->bats[index].type == ZBBATTYPE_TAZ)
	{
		yOffset=METERTOUNIT(0.5f);
		distFromBat=METERTOUNIT(2);
	}
	else// PP: gossamer
	{
		yOffset=METERTOUNIT(1.2f);

		if(gameStatus.multiplayer.numScreens == 2)
		{
			distFromBat=METERTOUNIT(4);
		}
		else
		{
			distFromBat=METERTOUNIT(3);
		}
	}

	SETVECTOR(tempVec, 0.0f, yOffset, distFromBat, 1.0f);

	bmVectorRotateY(camPos, tempVec, PI-this->bats[index].getYang());

	bmVectorAdd(camPos, camPos, this->bats[index].pos());

	this->outroCamPos[index]=camPos;

	SetCameraPosition(cam, camPos);

	SETVECTOR(lookat, this->bats[index].pos()[X], this->bats[index].pos()[Y]+yOffset, this->bats[index].pos()[Z], 1.0f);

	this->outroCamLookat[index]=lookat;

	SetCameraLookAtPosition(cam, lookat);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::setCamera
	Purpose 	: set the camera shot
	Parameters 	: shot to set
	Returns 	: 
	Info 		: 
*/

#define ZBCAM_INTROHOOT_CLOSEUP_FOV		RAD(70.0f)
#define ZBCAM_INGAME_FOV_1PLAYER		RAD(74.5f)// PP: RAD(85.0f)
#define ZBCAM_INGAME_FOV_2PLAYER		RAD(56.0f)

void ZOOBOSSGAME::setCamera(const EZooBossCamera cam)
{
	switch(cam)
	{
	case ZBCAM_INTRODROP:
		{
			setNumScreens(1);

			// PP: no actor-instance following for cam pos
			SetCameraPosFollowActorInstance(&camera[0],NULL,NULL);

			// PP: none of this
			SetFixedDistanceToLookAt(&camera[0], 0.0f);

			// PP: only the flags we need
			camera[0].flags=CAMERA_NOCOLLISION|CAMERA_RUMBLEFADE;

			// PP: don't want lerping neivah
			SetCameraSmooth(&camera[0], 1.0f, 1.0f);

			// PP: ~~~~~~~~~~~~~~~~~~~~~~~~

			// PP: set camera pos
			TBVector	camPos;
			//SETVECTOR(camPos, -863.66f, ZBBALL_INTROID_HEIGHT, 1119.26f, 1.0f);
			SETVECTOR(camPos, 227.439f, ZB_COURT_HEIGHT+METERTOUNIT(2), 1096.16f, 1.0f);
			SetCameraPosition(&camera[0], camPos);

			// PP: don't want lerping
			SetCameraSmooth(&camera[0], 1.0f, 1.0f);

			// PP: lookat follows the elephant
			SetCameraLookAtFollowActorInstance(&camera[0], this->ball.actorInstance, &(this->ball.actorInstance->actorInstance), (char*)NULL);
		}
		break;

	case ZBCAM_INTROHOOT:
		{
			setNumScreens(1);

			// PP: no actor-instance following
			DisableCameraLookAtFollowActorInstance(&camera[0]);
			SetCameraPosFollowActorInstance(&camera[0],NULL,NULL);

			// PP: none of this
			SetFixedDistanceToLookAt(&camera[0], 0.0f);

			// PP: only the flags we need
			camera[0].flags=CAMERA_NOCOLLISION|CAMERA_RUMBLEFADE;

			// PP: don't want lerping neivah
			SetCameraSmooth(&camera[0], 1.0f, 1.0f);

			// PP: ~~~~~~~~~~~~~~~~~~~~~~~~

			// PP: use biggish fov to exaggerate elephant's size, while not distorting the shape so much that it doesn't look like an elephant
			setFOV(ZBCAM_INTROHOOT_CLOSEUP_FOV);

			// PP: camera position

			TBVector	camPos;
			TBVector	tempVec;

			SETVECTOR(tempVec, 0.0f, METERTOUNIT(1.0f), METERTOUNIT(7.5), 1.0f);
			bmVectorRotateY(camPos, tempVec, PI-this->ball.getYang());
			bmVectorAdd(camPos, camPos, this->ball.pos());

			SetCameraPosition(&camera[0], camPos);

			// PP: camera lookat

			TBVector	lookat;
			SETVECTOR(lookat, this->ball.pos()[X], this->ball.pos()[Y]+METERTOUNIT(3.0f), this->ball.pos()[Z], 1.0f);
			SetCameraLookAtPosition(&camera[0], lookat);
		}
		break;

	case ZBCAM_PLAY:
		{
			// PP: no camera case
	//		StopCameraCase(&camera[0], this->debugCameraCase);
	//		StopCameraCase(&camera[0], this->cameraCase);

			// JW: Turn off the stars on the loser
			DestroyInjuryStars(ZooBoss_game.bats[ZooBoss_game.loser].actorInstance);

			// PP: like, set the right number of screens for play, man
			if(ZooBoss_game.twoPlayer)
			{
				// PP: two screens
				setNumScreens(2, SPLITSCREEN_HORIZONTAL);

				setFOV(ZBCAM_INGAME_FOV_2PLAYER);
			}
			else
			{
				// PP: NEW TEST
				setFOV(ZBCAM_INGAME_FOV_1PLAYER);
			}

			// PP: set smoothing on all cameras
			for(int i=0; i<gameStatus.multiplayer.numScreens; i++)
			{
				camera[i].flags=CAMERA_NOCOLLISION|CAMERA_RUMBLEFADE;

				// PP: both cameras (if there are two) follow a bat each and both point at the ball

				SetCameraPosFollowActorInstance(&camera[i],&gameStatus.player[i].actorInstance->actorInstance,NULL);

				SetCameraSmooth(bats[i].cam, 0.85f, 0.06f);

// PP: 				bkPrintf("FEUV %5.2f\n", videoMode.screens[i].fov);
			}
		}
		break;

	case ZBCAM_GOALWINNER:
		{
			// PP: close both scare prompts
			for(int i=0; i<BOSS_NUM_PLAYERS; i++)
			{
				this->bats[i].book.close();
			}

			// PP: into one-screen for this bit
			setNumScreens(1);

			// PP: low fov to stop character looking enormous
			// PP: REMOUT: now in cameraLookAtBat	setFOV(ZBCAM_GOAL_CLOSEUP_FOV);

			// PP: ~~~~~~~~~~~~~~~~~~~~~~~~

			this->cameraLookAtBat(ZooBoss_game.winner);
		}
		break;

	case ZBCAM_GOALLOSER:
		{
			// PP: into one-screen for this bit
			setNumScreens(1);

			// PP: TEST!
			if(ZooBoss_game.bats[ZooBoss_game.winner].getScore() < this->getMaxScore())
			{
				ZooBoss_game.bats[ZooBoss_game.loser].setState(ZBBATSTATE_LOSING);
			}
			else
			{
				ZooBoss_game.bats[ZooBoss_game.loser].setState(ZBBATSTATE_LOST);
			}

			// PP: NOTE: assuming ZBCAM_GOALWINNER came first and so set up some camera stuff
			this->cameraLookAtBat(ZooBoss_game.loser);

		}
		break;

	case ZBCAM_2PLAYER_OUTRO:
		{
			// PP: gonna make this just like the westboss 2player outro shot, no time to experiment

			setNumScreens(2, SPLITSCREEN_VERTICAL);

			this->cameraLookAtBat(0);// PP: taz's camera looks at taz
			this->cameraLookAtBat(1);// PP: gossamer's camera looks at gossamer
		}
		break;
	}

	this->camShot=cam;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::updateCamera
	Purpose 	: update the Zoo boss game's camera or cameras
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define ZBCAM_MIN_Y_OFFSET_TAZ			METERTOUNIT(0.8f)		// PP: minimum y offset from bat
#define ZBCAM_MAX_Y_OFFSET_TAZ			((gameStatus.multiplayer.numScreens==1)?METERTOUNIT(3.0f):METERTOUNIT(2.0f))// PP: maximum y offset from bat

#define ZBCAM_MIN_Y_OFFSET_GOSSAMER		METERTOUNIT(1.9f)		// PP: minimum y offset from bat
#define ZBCAM_MAX_Y_OFFSET_GOSSAMER		METERTOUNIT(3.0f)		// PP: maximum y offset from bat

// PP: ~~~~~~~							
#define ZBCAM_MIN_RADIUS				METERTOUNIT(1.75f)		// PP: minimum x-z radius around bat
#define ZBCAM_MAX_RADIUS				METERTOUNIT(3.5f)		// PP: maximum x-z radius around bat
										
#define ZBCAM_MIN_RADIUS_SPLIT			ZBCAM_MIN_RADIUS		// PP: minimum x-z radius around bat
#define ZBCAM_MAX_RADIUS_SPLIT			ZBCAM_MIN_RADIUS		// PP: maximum x-z radius around bat
// PP: ~~~~~~~							
#define ZBCAM_MIN_DIST					METERTOUNIT(4.0f)		// PP: near extreme ball distance for interpolation
#define ZBCAM_MAX_DIST					METERTOUNIT(13.0f)		// PP: far extreme ball distance for interpolation



void ZOOBOSSGAME::updateCamera(void)
{
	switch(this->camShot)
	{
	case ZBCAM_PLAY:
		{
			for(int i=0; i<gameStatus.multiplayer.numScreens; i++)
			{
				// PP: UPDATE CAMERA OFFSET TO SWING ROUND TAZ AND LOOK AT ELEPHANT

				TBVector	destLookatPos;

				SETVECTOR(destLookatPos, this->ball.pos()[X], ZB_COURT_HEIGHT+METERTOUNIT(1.5f), this->ball.pos()[Z], 1.0f);

				SetCameraLookAtDestination(this->bats[i].cam, destLookatPos);

				TBVector	dist;
				TBVector	offset;

				float		camMinYOffset;
				float		camMaxYOffset;

				if(this->bats[i].type == ZBBATTYPE_TAZ)
				{
					camMinYOffset=ZBCAM_MIN_Y_OFFSET_TAZ;
					camMaxYOffset=ZBCAM_MAX_Y_OFFSET_TAZ;
				}
				else// PP: gossamer
				{
					camMinYOffset=ZBCAM_MIN_Y_OFFSET_GOSSAMER;
					camMaxYOffset=ZBCAM_MAX_Y_OFFSET_GOSSAMER;
				}

				bmVectorSub(dist, destLookatPos, this->bats[i].pos());

				float		frac=(bmVectorLen(dist)-ZBCAM_MIN_DIST)/(ZBCAM_MAX_DIST-ZBCAM_MIN_DIST);

				frac=CONSTRAIN(frac, 0.0f, 1.0f);

				float radius=ZBCAM_MIN_RADIUS+(frac*(ZBCAM_MAX_RADIUS-ZBCAM_MIN_RADIUS));

				bmVectorScaleToLength(offset, dist, -radius);

				offset[Y]=camMinYOffset+(frac*(camMaxYOffset-camMinYOffset));

				TBVector	destCameraPos;

				bmVectorAdd(destCameraPos, this->bats[i].pos(), offset);

#ifdef ZB_CONTAIN_CAMERA// PP: ZooBoss.cpp

				// PP: NEW TEST: contain the camera position within the XZ bounds of the court
				{
#define ZBCAM_COURT_CONTAIN_MARGIN		METERTOUNIT(1.0f)

					destCameraPos[Z]=CONSTRAIN(destCameraPos[Z], (this->court.bottom+ZBCAM_COURT_CONTAIN_MARGIN), (this->court.top-ZBCAM_COURT_CONTAIN_MARGIN));
				}
#endif// PP: def ZB_CONTAIN_CAMERA

				SetCameraPositionDestination(this->bats[i].cam, destCameraPos);
			}
		}
		break;

	case ZBCAM_GOALWINNER:
		{
			if(this->bats[this->winner].animChanged)
			{
				// PP: winner has finished celebration anim - switch to luzer
				this->setCamera(ZBCAM_GOALLOSER);
			}
		}
		break;

	case ZBCAM_GOALLOSER:
		{
			// PP: TEST

			if(!g_forceResult)// PP: can't think what this was for
			{
				if(ZooBoss_game.bats[ZooBoss_game.loser].animChanged)
				{
					if(ZooBoss_game.bats[ZooBoss_game.loser].state == ZBBATSTATE_LOSING)// PP: rather than lost
					{
						this->ball.setState(ZBBALLSTATE_GOALDROP);

						for(int p=0;p<BOSS_NUM_PLAYERS;p++)
						{
							this->bats[p].setState(ZBBATSTATE_PLAYING);
						}
					}
				}
			}
		}
		break;

	case ZBCAM_2PLAYER_OUTRO:
		{
			for(int i=0; i<BOSS_NUM_PLAYERS; i++)
			{
				SetCameraPosition(&camera[i], this->outroCamPos[i]);
				SetCameraLookAtPosition(&camera[i], this->outroCamLookat[i]);
			}
		}
		break;
	}
}

#define FULLSCREEN_BOOK_RECT		RECTANGLE(-0.5f/0.95f, 0.5f/0.95f, -0.5f/0.95f, 0.5f/0.95f)


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::update
	Purpose 	: update the Zoo Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::update
*/
void ZOOBOSSGAME::update(void)
{
	BG_START_UPDATE;

	// PP: base-class update
	BOSSGAME::update();

	int		b;		// PP: loop counter
	// PP: TEMP TEST
//	fps=60;

	// PP: OH SO TEMP!!!!!! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
	TBVector	tempVec;
	bmVectorSub(tempVec, this->ball.pos(), this->bats[0].pos());
	bmVectorNorm(ZooBoss_sunDir, tempVec);

	// PP: TEMP?: set up the light direction
	UpdateCartoonLight(ZooBoss_sunDir, COLOUR(127, 100, 64));				// PP: this is ok - InitCartoonLight has been called earlier on in InitialiseScene
*/
	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	// PP: DEBUGGY STUFF... - 

/*	if(this->cameraCase->KOScamData != NULL)
	{
		this->cameraCase->KOScamData->angle[Y]=Aabs(this->cameraCase->KOScamData->angle[Y]+(controller1.x2*0.1f));
		this->cameraCase->KOScamData->angle[X]=Aabs(this->cameraCase->KOScamData->angle[X]+(controller1.y2*0.1f));
	}*/

	//bkPrintf("cam angle x%5.2f, y%5.2f  player pos: (x%5.2f) x%5.2f, %5.2f, z%5.2f\n", this->cameraCase->KOScamData->angle[X], this->cameraCase->KOScamData->angle[Y], this->bats[0].actorInstance->actorInstance.position[X], UNITTOMETER(this->bats[0].actorInstance->actorInstance.position[X]), UNITTOMETER(this->bats[0].actorInstance->actorInstance.position[Y]), UNITTOMETER(this->bats[0].actorInstance->actorInstance.position[Z]));

#ifdef ZOOBOSS_DEBUG// PP: if debug options are enabled

	if((controller1.r1DebounceChannel->value != 0)||(controller1.r2DebounceChannel->value != 0))
	{
		ZooBoss_debug ^= 1;
	}

	if(ZooBoss_debug)	
	{
		//StopCameraCase(&camera[0], this->cameraCase);
		StartCameraCase(this->debugCameraCase, &camera[0]);
	}
	else
	{
		StopCameraCase(&camera[0], this->debugCameraCase);
		//StartCameraCase(this->cameraCase, &camera[0]);
	}

#endif// PP: def ZOOBOSS_DEBUG// PP: if debug options are enabled

	// PP: rumble test
	if(controller1.r3DebounceChannel->value != 0)
	{

/*#define MAX_RUMBLE2		METERTOUNIT(6.0f)		// PP: christ he sounds like some wrestler

		TBVector		rumble;
		
		SETVECTOR(rumble, (bmRand()/RANDMAX)*MAX_RUMBLE2, (bmRand()/RANDMAX)*MAX_RUMBLE2, (bmRand()/RANDMAX)*MAX_RUMBLE2, 1.0f);
		RumbleCamLookat(this->cameraCase->camera, rumble);*/
	}

	// PP: update the ball
	this->ball.update();

	// PP: update the bats
	//for(int b=0; b<BOSS_NUM_PLAYERS; b++)
	for(b=BOSS_NUM_PLAYERS-1; b>=0; b--)
	{
		this->bats[b].update();
	}

	// PP: update sam
	this->sam.update();
 
	// PP: GAME UPDATE
	switch(this->state)
	{
		case BGSTATE_PLAY:
		{
			switch(this->ball.state)
			{
			case ZBBALLSTATE_RUNNING:
				{
					for(int p=0;p<BOSS_NUM_PLAYERS;p++)
					{
						if(this->ball.hear(this->bats[p]))
						{
							if(!this->ball.batHearLatch[p])	
							{
								if(this->twoPlayer || (p==0))
								{
									this->bats[p].book.open("SCARE", true);
								}
							}


							if(this->bats[p].isDeflecting(this->ball))
							{
								// PP: skip if the ball is already in the process of dealing with a collision with this bat
								if(!this->ball.batScareLatch[p])
								{
									this->ball.beDeflected(this->bats[p]);

									this->ball.batScareLatch[p]=true;
								}
							}

							this->ball.batHearLatch[p]=true;
						}
						else
						{
							if(this->ball.batHearLatch[p])
							{
								this->ball.batHearLatch[p]=false;
								this->ball.batScareLatch[p]=false;

								if(this->twoPlayer||(p==0))
								{
									this->bats[p].book.close();
									//bkPrintf("closing scare book\n");
								}
							}

						}

						if(this->ball.innerCollide(this->bats[p]))// PP: if the ball has this bat in its collision radius
						{	
							if(!this->bats[p].isDeflecting(this->ball))
							{
								// PP: skip if the ball is already in the process of dealing with a collision with this bat
								if(!this->ball.batInnerCollLatch[p])
								{
									this->bats[p].getHit(this->ball);
								}
							}
							
							this->ball.batInnerCollLatch[p]=true;
						}
						else
						{
							this->ball.batInnerCollLatch[p]=false;
						}		
					}
				}
				break;

			case ZBBALLSTATE_SKIDDING:
				{
					// PP: ball-bat collision
					for(int p=0;p<BOSS_NUM_PLAYERS;p++)
					{
					/*	if(this->ball.tuskCollide(this->bats[p]))
						{
							if(!this->ball.batTuskCollLatch[p])
							{
								this->bats[p].getProdded(this->ball);

								this->ball.batTuskCollLatch[p]=true;
							}
						}
						else
						{*/
							this->ball.batTuskCollLatch[p]=false;

							if(this->ball.outerCollide(this->bats[p]))
							{
								// PP: skip if the ball is already in the process of dealing with a collision with this bat
								if(!this->ball.batOuterCollLatch[p])
								{
									this->ball.batOuterCollLatch[p]=true;

									this->bats[p].getKnocked(&this->ball);
								}
							}
							else
							{
								this->ball.batOuterCollLatch[p]=false;
							}
					//	}
					}
				}
				break;

			case ZBBALLSTATE_GOAL:
				{
				/*	bool	finishedGoal=true;			// PP: used for determining if a goal that was in progress is still in progress

					for(b=0;b<BOSS_NUM_PLAYERS;b++)
					{
						finishedGoal=finishedGoal&&(!this->bats[b].playingAnim());
					}

					if(finishedGoal)
					{
						this->ball.setState(ZBBALLSTATE_GOALDROP);

						for(int p=0;p<BOSS_NUM_PLAYERS;p++)
						{
							this->bats[p].setState(ZBBATSTATE_PLAYING);
						}
					}*/
				}
				break;
			}
		}
		break;

		case BGSTATE_INTRO:
			{
				// PP: if the ball has finished doing its intro thangs, start the game proper
				if(this->ball.flags & ZBBALLFLAG_NEWSTATE)
				{
					if((this->ball.state >= ZBBALLSTATE_RUNNING)&&(this->ball.oldState < ZBBALLSTATE_RUNNING))
					{
						this->setState(BGSTATE_PLAY);
					}

					//this->ball.flags&= ~ZBBALLFLAG_NEWSTATE;
				}
			}
			break;

		case BGSTATE_OUTRO:
			{
				if(this->twoPlayer)
				{
					BOSSGAME::handle2PlayerOutroState();
				}
				else
				{
					// PP: aw I've lost track of what's going on
					this->setState(BGSTATE_CUTSCENE);
				}
			}
			break;

		case BGSTATE_CUTSCENE:
			// PP: if ((delay-=fTime)<=0.0f)
			if(this->bats[this->loser].animChanged)
			{
				// PP: hmm, this function scares me a bit - it should be used carefully
				BookList_closeAll();

			// PP: 	this->messageBook.close(0.0f, true);
				if(this->winner == 0)
				{
					LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
				}
			/* PP: REMOUT: OOD	else
				{
					bkGenerateEvent("scenechange","_zoohub");
				}*/

				// TP: Added this to stop repeated levelcomplete calls being made
				setState(BGSTATE_NONE);
			}
			break;
	}


	// PP: clear some of the ball's flags
	this->ball.flags&= ~(ZBBALLFLAG_NEWSTATE|ZBBALLFLAG_NEWDIRECTION|ZBBALLFLAG_NEWLYENTEREDCONTACTCOURT);
	
	// PP: TEMP TEST!!!!
	//if(this->state != BGSTATE_OUTRO)
	{
		// PP: Now that all the actors have been moved about, update the camera(s)
		this->updateCamera();
	}


	// PP: TEMP TEST: show player 1's position
//	ZooBoss_debugBox->sprintf("x %5.2f\\ny %5.2f\\nz %5.2f", this->bats[0].pos()[X], this->bats[0].pos()[Y], this->bats[0].pos()[Z]);

//	ZooBoss_debugBox->sprintf("%5.2f", this->ball.getYang());

	//bkPrintf("x %d, z %d\n", ROUND(UNITTOMETER(this->bats[0].pos()[X])), ROUND(UNITTOMETER(this->bats[0].pos()[Z])));


/*
	{
		// PP: TEMP
		// PP: point the bat towards the ball...
		float ang;
		TBVector2D dist;

		dist[X]=this->ball.pos()[X]-this->bats[0].pos()[X];
		dist[Y]=this->ball.pos()[Z]-this->bats[0].pos()[Z];

		ang=bmATan2(dist[Y], dist[X]);

		this->bats[0].setYang(Aabs(ang+HALFPI));
	}*/



	// PP: TEMP TEST
//	bmVectorCopy(this->bats[1].pos(), markerPos);
//	bmVectorCopy(markerPos, this->ball.pos());

// PP: temp test

	// PP: temp test
//	ZooBoss_debugBox->sprintf("X %5.2f, Z %5.2f", this->bats[0].pos()[X], this->bats[0].pos()[Z]);
	//ZooBoss_debugBox->sprintf("speed %5.2f", this->ball.speedFraction);
//	ZooBoss_debugBox->sprintf("%5.2f", ZooBoss_bookScale);

/*	if(controller1.crossChannel->value != 0)
	{
		ZooBoss_bookScale+=0.05f;
		this->scoreBook.setScale(ZooBoss_bookScale);
	}
	else if(controller1.circleChannel->value != 0)
	{
		ZooBoss_bookScale-=0.05f;
		this->scoreBook.setScale(ZooBoss_bookScale);
	}*/

	// PP: TEMP TEMP TEST - SLOW!!!!
// PP: 	this->scoreBook.realign();

#ifdef ZB_PONGDUST
	// PP: update dust
	PongDust_update();
#endif// PP: def ZB_PONGDUST
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::shutdown
	Purpose 	: shut-down the Zoo Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::shutdown
*/
void ZOOBOSSGAME::shutdown(void)
{
	// PP: do base-class shutdown
	BOSSGAME::shutdown();

#ifdef ZB_PONGDUST
	// PP: shutdown dust
	PongDust_sceneShutdown();
#endif// PP: def ZB_PONGDUST

	// PP: free the shadow zmask
	if(ZOOBOSSGAME::shadowZMask != NULL)
	{
		baFreeActorInstance(ZOOBOSSGAME::shadowZMask);
		// PP: REMOUT: ?: causing a crash hmm		FREE(ZOOBOSSGAME::shadowZMask);

		ZOOBOSSGAME::shadowZMask=NULL;
	}

	// PP: SET NORMAL GAME SPEED
	gameStatus.gameSpeed=1.0f;

	// PP: set fov back to the default
	setFOV(NORMAL_INGAME_FOV);

	// PP: set the light ramp back to the default
	SetLightRamp(LIGHTS_DEFAULT_RAMP);

	// PP: back to normal shadows - I've had my fun
	if(!CheatActive(CHEAT_SHADOWMAP))
	{
		VFX_deactivateEffect(VFX_SHADOWMAP);
	}

	// PP: shut down the keep-on-screen camera's target list
// PP: 	cameraTargetList.shutdown();// PP: shutdown the target list for the camera

	// PP: TODO: shut this stuff down when it goes into the outro cutscene

	// PP: shutdown bats - kinda important no?
	for(int i=0; i<BOSS_NUM_PLAYERS; i++)
	{
		this->bats[i].shutdown();
	}

	// PP: shutdown sam
	this->sam.shutdown();

	// PP: clean up camera case
	StopCameraCase(&camera[0], this->debugCameraCase, false);
	DeleteCameraCase(this->debugCameraCase);

#ifdef ZB_SPINOFF

	// PP: stop any spin-off in progress
	VFX_deactivateEffect(VFX_SPINOFF);

#endif// PP: def #ifdef ZB_SPINOFF

	// PP: NEW TEST MS FRIG
/* PP: REMOUT: shouldn't need this now	if(this->twoPlayer)
	{
		gameStatus.multiplayer.numPlayers=1;
		setNumScreens(1);
	}*/

	// PP: restore normal splitscreen mode
	gameStatus.multiplayer.splitScreenMode=SPLITSCREEN_HORIZONTAL;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::groundPoundCameraShake
	Purpose 	: shake the camera in response to something pounding the ground
	Parameters 	: position of impact, strength of pound, (opt/0.0f)maximum random deviation (absolute)
	Returns 	: 
	Info 		: FOR GOD'S SAKE, STRAP YOURSELVES DOWN!!
*/
void ZOOBOSSGAME::groundPoundCameraShake(TBVector pos, const float strength, const float maxDeviation)
{
#define ZBCAM_MAX_ANG_RUMBLE			METERTOUNIT(0.0025f)

#define ZBCAM_MAX_POS_RUMBLE			METERTOUNIT(1.5f)

#define ZBCAM_RUMBLE_RANGE				METERTOUNIT(25)

	// PP: MOVEME to init


	TBVector		rumble;				// PP: rumble vector
	TBVector		distVec;			// PP: vector distance between camera and pound point
	float			distance;			// PP: scalar distance between camera and pound point
	float			xShare, zShare;		// PP: x & z portions of rumble
	float			deviation;


	for(int i=0; i<gameStatus.multiplayer.numScreens; i++)
	{
		bmVectorSub(distVec, pos, this->bats[i].cam->pos);
		distance=bmVectorLen(distVec);

		//ZooBoss_debugBox->sprintf("dist %5.2f", distance);
		
		if(maxDeviation != 0.0f)
		{
			deviation=(bmRandf()-0.5f)*(maxDeviation*2.0f);
		}

		// PP: get x&z shares...

		xShare=0.5f+(bmRandf()*0.5f);// PP: bias towards x tilt cos its sweeter
		zShare=1.0f-xShare;
		
		// PP: shake the camera angle... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		SETVECTOR(rumble, strength*xShare/*((bmRand()&1)?1.0f:(-1.0f))**/*ZBCAM_MAX_ANG_RUMBLE,
							0.0f,
							strength*zShare*((bmRand()&1)?1.0f:(-1.0f))*ZBCAM_MAX_ANG_RUMBLE,
							1.0f);

		// PP: take into account random deviation
		if(maxDeviation != 0.0f)
		{
			bmVectorCMul(rumble, rumble, 1.0f+deviation);
		}

		// PP: take into account distance falloff
		bmVectorCMul(rumble, rumble, 1.0f-MIN(1.0f, (distance/ZBCAM_RUMBLE_RANGE)));

		// PP: apply the rumble to the camera angle
		RumbleCamAng(this->bats[i].cam, rumble);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::spitCollide
	Purpose 	: respond to collision with a spat object
	Parameters 	: projectile, victim
	Returns 	: 
	Info 		: 
*/
int ZOOBOSSGAME::spitCollide(ACTORINSTANCE *collider, ACTORINSTANCE *object)
{
	if(collider->flags & ACTORFLAG_DEBRISACTOR)
	{
		// PP: TODO: see if it's a ball

		// PP: see if it's a bat
		for(int b=0; b<BOSS_NUM_PLAYERS; b++)
		{
			if(object == ZooBoss_game.bats[b].actorInstance)
			{
				ZooBoss_game.bats[b].getKnocked();
			}
		}

		return FALSE;
	}

	return FALSE;
}



/* --------------------------------------------------------------------------------
   Function		: ZOOBOSSGAME::fadeDownCallback
   Purpose		: function called when the fade-down, before the continue sequence, finishes
   Parameters	: 
   Returns		: 
   Info			: // PP: must match the BossGameFadeDownCallback typedef (bossgamecontinue.h)
*/
void ZOOBOSSGAME::fadeDownCallback(void)
{
	// PP: GOTTER RESET A FEW THINGS BEFORE GOING INTO CONTINUE SEQUENCE...

	// PP: SET NORMAL GAME SPEED
	gameStatus.gameSpeed=1.0f;

	// PP: set fov back to the default
	setFOV(NORMAL_INGAME_FOV);

	// PP: set the light ramp back to the default
	SetLightRamp(LIGHTS_DEFAULT_RAMP);

	// PP: deactivating shadowmaps resets light dir to "normal"
	VFX_deactivateEffect(VFX_SHADOWMAP);
}


/* --------------------------------------------------------------------------------
   Function		: ZOOBOSSGAME::resetCallback
   Purpose		: function called when the player selects the option to continue or to quit
   Parameters	: context value passed through from ProcessTazDeathAtEndOfBossGame,
					the player's choice of action (BOSSGAMERESET_CONTINUE, BOSSGAMERESET_QUIT)
   Returns		: big boobs
   Info			: // PP: must match the BossGameResetCallback typedef (bossgamecontinue.h)
*/
int32 ZOOBOSSGAME::resetCallback(void *context, EBossGameResetChoice choice)
{
	switch(choice)
	{
	case BOSSGAMERESET_CONTINUE:
		bkGenerateEvent("scenechange", "_reset");
		break;
	case BOSSGAMERESET_QUIT:
		// PP: // CMD: all we need to do is reset back to the frontend
		bkGenerateEvent("scenechange", "_zoohub");
		break;
	}

	// PP: must return a value
	return 0xB16B00B5;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::ruleBookOpenFunc
	Purpose 	: page-open func for zooboss page of the rules paperbook
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void ZOOBOSSGAME::ruleBookOpenFunc(struct TAG_PAGE* const page, const int32 context)
{
	// PP: default page open func for rule book pages
	extern void MP_RuleBook_pageOpenFunc(struct TAG_PAGE* const page, const int32 context);// PP: multiplayer.cpp

	// PP: call the default page open func - fixes up controller stuff
	MP_RuleBook_pageOpenFunc(page, context);

	// PP: close the message book in case it's open on the ID page
	ZooBoss_game.messageBook.close(0.0f, true);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														ZBBAT methods

								These are the 'bats' (Taz & Gossamer) used in the Zoo Boss game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*

		******* BAT AI ALGORITHM ********

		* get the next set of forward-facing path lines of the ball in this side of the court

		* for each one, get the point of perpendicular intersection with the bat
		OR
		* for each one, get the point of intersection if we just move sideways - TRY THIS ONE FIRST IT'S SO MUCH SIMPLER!

		* is that point within the court?
		* is that point close enough that we can move to it in time / is it the one that allows most time?
		* if so, move out towards that point
		* is the ball close enough to scare?

*/


// PP: MOVEME?
#define ZBBAT_NO_CURRENT_ANIM		(!CheckIfPlayingAnimation(this->actorInstance, NULL))
#define ZBBAT_CURRENT_ANIM(_anim)	(CheckIfPlayingAnimationPointedTo(this->actorInstance->actorInstance, this->anims[#_anim]))

#define ZBBATAI_TIME_TO_SCARE		1.2f						// PP: Time left by bat AI for scaring the ball before it is expected to hit the bat
#define ZBBATAI_SCARE_DISTANCE		METERTOUNIT(3.5f)// PP: not used

// PP: initialise static members...

// PP: AI run-speed factors for each of the different difficulty levels.  By slowing Gossamer down, we we can make the game easier.
float	ZBBATAI::runSpeedFactors[NUM_DIFFICULTY_LEVELS]={0.4f,		// PP: easy
														0.5f,		// PP: medium
														0.6f};		// PP: hard

float ZBBALL::initialSpeedFractions[NUM_DIFFICULTY_LEVELS]={0.4f,	// PP: easy
															0.6f,	// PP: medium
															0.8f};	// PP: hard
/*
float	ZBBATAI::runSpeedFactors[NUM_DIFFICULTY_LEVELS]={0.7f,		// PP: easy
														0.95f,		// PP: medium
														1.175f};		// PP: hard

float ZBBALL::initialSpeedFractions[NUM_DIFFICULTY_LEVELS]={0.4f,	// PP: easy
															0.6f,	// PP: medium
															0.8f};	// PP: hard
*/
/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::sulk
	Purpose 	: sulk (other player has scored)
	Parameters 	: ref to opponent who has scored
	Returns 	: 
	Info 		: 
*/
void  ZBBAT::sulk(struct TAG_ZBBAT& winner)
{
	// PP: TODO: turn towards crack in the wall

	// PP: NEW TeST - should take taz out of spin
	// PP: RequestStateChange(this->actorInstance,STATE_MOVE);

	if(this->player != NULL)
	{
		SpinDown(this->player);
	}

	this->setYang(this->normalYang);

	// PP: is this the bat's final sulk in 2player?
	bool final2P=(ZooBoss_game.twoPlayer && (winner.getScore() == ZB_MAX_SCORE_2PLAYER));

	// PP: play oh-bugger anim...

	if(this->type == ZBBATTYPE_TAZ)
	{
		this->playAnim("unhappytaz", final2P);			// PP: play sulk anim
	}
	else// PP: if(this->type == ZBBATTYPE_GOSSAMER)
	{
		if(final2P)
		{
			this->playAnim("defeat");			// PP: slap face, shake head, collapse
			this->queueAnim("tantrum", true);	// PP: lying on ground in tantrum
		}
		else
		{
			if(bmRand()&1)
			{
				this->playAnim("defeat1");
			}
			else
			{
				this->playAnim("defeat2");
			}
		}
	}

	if(!final2P)
	{
		this->queueAnim(this->anims.idle);		// PP: and queue a blend back into idle
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::updateScoreDisplay
	Purpose 	: display updated score
	Parameters 	: 
	Returns 	: 
	Info 		: not as silly as it looks
*/
inline void ZBBAT::updateScoreDisplay(void)
{
	this->scoreBox->sprintf8("%d/%d", this->getScore(), ZooBoss_game.getMaxScore());
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::increaseScore
	Purpose 	: increase the bat's score
	Parameters 	: 
	Returns 	: 
	Info 		: not as silly as it looks
*/
void ZBBAT::increaseScore(void)
{
	// PP: TEMP
	this->score++;

	this->updateScoreDisplay();
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::celebrate
	Purpose 	: celebrate scoring a goal
	Parameters 	: ref to opponent against whom a goal was scored
	Returns 	: 
	Info 		: 
*/
void ZBBAT::celebrate(ZBBAT& loser)
{
	// PP: TODO: turn towards loser

	if(this->player != NULL)
	{
		SpinDown(this->player);
	}

	this->setYang(this->normalYang);

	// PP: audience applause
	PlaySample("applause.wav");

	// PP: is this the bat's final celebration in 2player?
	bool final2P=(ZooBoss_game.twoPlayer && (this->getScore() == ZB_MAX_SCORE_2PLAYER));

	// PP: play victory anim...

	if(this->type == ZBBATTYPE_TAZ)
	{
		this->playAnim("happytaz", final2P);
	}
	else// PP: if(this->type == ZBBATTYPE_GOSSAMER)
	{
		if(final2P)
		{
			this->playAnim("victory3");			// PP: jump up & spin
			this->queueAnim("tazdie", true);	// PP: laugh & bow repeatedly
		}
		else
		{
			if(bmRand()&1)
			{
				this->playAnim("victory1");
			}
			else
			{
				this->playAnim("victory2");
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::init
	Purpose 	: initialise the bat
	Parameters 	: y angle, court rectangle, ptr to wall in ball's court to protect, type for bat,
					ptr to tex box for score, (opt/NULL=cpu)player struct
	Returns 	: 
	Info 		: 
*/
#define ZBBAT_RADIUS		METERTOUNIT(0.5f)			// PP: move & comment me

// PP: movement speed when the bat is playing its run anim (at normal speed, unblended)
#define ZBBAT_RUN_SPEED_GOSSAMER	METERTOUNIT(5.5f)			// PP: max dist per second
#define ZBBAT_RUN_SPEED_TAZ			METERTOUNIT(4.497f)			// PP: max dist per second

void ZBBAT::init(const float yAng, const RECTANGLE& courtIn, float* const wallIn, const ZBBatType typeIn, TEXTBOX* const scoreBoxIn, PLAYER* const playerIn)
{
	char*		gossamerModel="gossamer.obe";
	char*		tazModel="tazdevil.obe";
	char*		model;
	TBVector	pos;

	// PP: set the court for the bat
	this->court=courtIn;

	// PP: get position for bat: centre of bat's court
	SETVECTOR(pos, this->court.midX(), ZB_COURT_HEIGHT, this->court.midY(), 1.0f);

	// PP: set type for bat
	this->type=typeIn;

	// PP: set text box for score
	this->scoreBox=scoreBoxIn;
	this->score=0;						// PP: START FROM NIL-NIL LIKE IN FOOTBALL

	this->updateScoreDisplay();

	this->allowMove=true;

	this->runPhase=0.0f;
	this->idlePhase=0.0f;

	this->speedFractions[X]=0.0f;
	this->speedFractions[Y]=0.0f;

	this->state=ZBBAT_INITIAL_STATE;

	this->player=playerIn;

	// PP: temp?

	if(this->player == NULL)
	{
		this->noiseLevel=0.8f;
	}
	else
	{
		this->noiseLevel=1.0f;

		// PP: clear the player's default control flag, so's we can use special zooboss control
		this->player->flags &= ~PLAYERFLAG_DEFAULTCONTROL;
	}

	// PP: SET UP PLAYER'S BOOK...

	PAGE*		page;
	TEXTBOX*	textBox;

	// PP: VERY IMPORTANT!  MEUH!!!
	this->book.init();

	// PP: SCARE PROMPT PAGE
	(page=this->book.addPage(textBox=new TEXTBOX(STR_ZOOBOSS_SCAREPROMPT), NULL, "SCARE"))
		->setEffectChooser(&racePIEChooser);
	textBox->setVGrad(false);
	textBox->setColour(COLOUR(128, 128, 128, 128));// PP: maybe try some alpha

	float	shadowMapRadius;

	if(typeIn==ZBBATTYPE_TAZ)
	{
		model=tazModel;
		shadowMapRadius=SHADOWMAP_RADIUS_TAZ;
	}
	else if(typeIn==ZBBATTYPE_GOSSAMER)
	{
		model=gossamerModel;
		shadowMapRadius=SHADOWMAP_RADIUS_GOSSAMER;
	}

	if(this->player == NULL)
	{
		// PP: set camera
		this->cam= &camera[0];

		// PP: create the actor instance for the bat, and set its position
		this->actorInstance=ZooBoss_game.createActorInstance(model, pos, yAng, BGACT_COLLCACHE|BGACT_ANIMATION|BGACT_BEHAVIOUR|BGACT_CARTOON);

		float			radius=250.0f;
		TBVector		offset={0,radius,0,0};
		bmVectorCopy(g_circleOffset, offset);
		g_circleRadius=radius;

		SetActorCollSphere(this->actorInstance->actorBehaviour, offset, radius);

		//AddActorToCollisionSphereCache(&collisionCache, this->actorInstance);
	}
	else
	{
		// PP: set camera
		this->cam=this->player->camera;

		// PP: set the actor instance for the bat
		this->actorInstance=this->player->actorInstance;

		// PP: turn off motion bone to prevent nasty things
		this->actorInstance->actorAnimation->useMotionBone=FALSE;

		// PP: TEMP: use default occluder radius. TODO: you know what you have to do.
		CreateActorInstanceShadow(this->actorInstance,METERTOUNIT(1.2f),METERTOUNIT(10), SHADOWMAP_DEFAULT_OCCLUDER_RADIUS, SHADOWTYPE_ZOOBOSS);

		// PP: set the position for the bat
		SetActorPosition(this->actorInstance, pos);
	}

	// PP: create shadow for bat...

	if(VFX_effectActive(VFX_SHADOWMAP))
	{
		CreateActorInstanceShadow(this->actorInstance,METERTOUNIT(0.7f),METERTOUNIT(1.5),shadowMapRadius);
	}
	else
	{
		CreateActorInstanceShadow(this->actorInstance,METERTOUNIT(1.4f),METERTOUNIT(10), shadowMapRadius, SHADOWTYPE_ZOOBOSS);
	}

	// PP: Set spit collision response
	SetCollisionCallback(this->actorInstance, ZOOBOSSGAME::spitCollide);

	// PP: set camera
/* PP: REMOUT: TEMP TEST	if(this->cam->cameraCase != NULL)
	{
		this->cam=this->cam->cameraCase->camera;
	}*/

	// PP: SET UP AI
	bmVector2DCopy(this->ai.destPos, this->pos());
	bmVector2DCopy(this->ai.ballPosOnScare, this->pos());// PP: ?
	this->ai.doneScare=false;
	this->ai.eta=0.0f;
	this->ai.timeToScare=ZBBATAI_TIME_TO_SCARE;
	this->ai.interceptLine.start[X]=0.0f;
	this->ai.interceptLine.start[Y]=0.0f;
	this->ai.interceptLine.end[X]=0.0f;
	this->ai.interceptLine.end[Y]=0.0f;
	this->ai.runSpeedFactor=ZBBATAI::runSpeedFactors[CURRENT_DIFFICULTY];

	// PP: set-up anims
	this->anims.run=baFindAnimSegment(&(this->actorInstance->actorInstance), "run");
	this->anims.runSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runstop");

	if(this->type == ZBBATTYPE_TAZ)
	{
		this->anims.runBack=baFindAnimSegment(&(this->actorInstance->actorInstance), "runback");
		this->anims.runBackSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runbackstop");
		this->anims.knocked=baFindAnimSegment(&(this->actorInstance->actorInstance), "react2");
		this->anims.prodded=baFindAnimSegment(&(this->actorInstance->actorInstance), "waterreact");
		this->anims.getup=baFindAnimSegment(&(this->actorInstance->actorInstance), "getup");
		this->runSpeed=ZBBAT_RUN_SPEED_TAZ;
	}
	else
	{
		this->anims.runBack=baFindAnimSegment(&(this->actorInstance->actorInstance), "runback");
		this->anims.runBackSkid=this->anims.runSkid;
		this->anims.knocked=baFindAnimSegment(&(this->actorInstance->actorInstance), "flattened");// PP: TEMP: TODO: ask nicely for a knocked anim?
		this->anims.prodded=baFindAnimSegment(&(this->actorInstance->actorInstance), "flattened");// PP: TEMP: TODO: ask nicely for a proded anim?
		this->anims.getup=NULL;
		this->runSpeed=ZBBAT_RUN_SPEED_GOSSAMER;
	}

	this->anims.strafeRight=baFindAnimSegment(&(this->actorInstance->actorInstance), "runright");
	this->anims.strafeRightSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runrightstop");

	this->anims.strafeLeft=baFindAnimSegment(&(this->actorInstance->actorInstance), "runleft");
	this->anims.strafeLeftSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runleftstop");

	this->anims.idle=baFindAnimSegment(&(this->actorInstance->actorInstance), "idle1");

	// PP: set y angle
	this->setNormalYang(yAng);
 	this->resetYang();

	// PP: set wall to protect
	this->wall=wallIn;

	this->moveYangRelative=0.0f;

	// PP: set bat radius
	this->radius=ZBBAT_RADIUS;

	// PP: set default scare type
	this->scareType=SCARE_SHOUT;

	// PP: flush the node queue to stop nasty things happening
	baFlushNodeQueue(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance, TRUE);

#ifdef ZBBAT_HEADTRACK
	// PP: prepare the waist bone to be twisted in code
	this->waistBone=baFindNode(this->actorInstance->actorInstance.rootNodeInstance, "spinemid");
	this->waistBone->flags &= ~BNODEINSTFLAG_ORIENTKEYS;
#endif// PP: def ZBBAT_HEADTRACK

	// PP: set nothing to look at
	this->lookAt(NULL);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::draw
	Purpose 	: draw special stuff the bat
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBAT::draw(void)
{
	if(this->player != NULL)
	{
		return;// PP: no use to me
	}

	if(ZooBoss_debug)
	{
		bdSetIdentityObjectMatrix();

		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		// PP: draw intercept line

		TBPrimVertex		intLineVerts[2];

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		bdSetTexture(0, NULL);

		if(bmVector2DDistance(this->ai.interceptLine.start, this->ai.interceptLine.end))
		{
			int c=0;
		}

		this->court.draw(RECTANGLE_DEFAULT_DRAW_COLOUR, RECTANGLE_DEFAULT_DRAW_FLAGS, X, Z, ZB_COURT_HEIGHT);

		// PP: add the start point of this line segment to the list of verts to be drawn in debug mode
		BDVERTEX_SETXYZW(&intLineVerts[0], this->ai.interceptLine.start[X], ZB_COURT_HEIGHT, this->ai.interceptLine.start[Y], 0.0f);
		//BDVERTEX_SETPOS(&intLineVerts[0], ZooBoss_game.bats[0].pos());
		BDVERTEX_SETRGBA(&intLineVerts[0], 255, 255, 255, 255);

		// PP: add the end point of this line segment to the list of verts to be drawn in debug mode
		BDVERTEX_SETXYZW(&intLineVerts[1], this->ai.interceptLine.end[X], ZB_COURT_HEIGHT, this->ai.interceptLine.end[Y], 0.0f);
		//BDVERTEX_SETPOS(&intLineVerts[1], ZooBoss_game.bats[1].pos());
		BDVERTEX_SETRGBA(&intLineVerts[1], 255, 255, 255, 255);

		bdDrawPrimitive(BDPRIMTYPE_LINELIST, intLineVerts, 2, BVERTTYPE_SINGLE);
		
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		utilDrawSphere(VEC(this->ai.ballPosOnScare[X], ZB_COURT_HEIGHT, this->ai.ballPosOnScare[Y]), 100.0f, RED);
		utilDrawSphere(VEC(this->ai.destPos[X], ZB_COURT_HEIGHT, this->ai.destPos[Y]), 100.0f, BLUE);
	}



	//bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	// PP: DRAW BOOK
// PP: REMOUT: Now handled by the Book List			this->book.draw();

	//bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::setXZVelocity
	Purpose 	: set the bat's velocity on the x-z plane
	Parameters 	: x component of velocity, z component of velocity
	Returns 	: 
	Info 		: 
*/
void ZBBAT::setXZVelocity(const float vx, const float vz)
{
	if(this->actorInstance->actorBehaviour != NULL)
	{
		this->actorInstance->actorBehaviour->physics.velocity[X]=vx;
		this->actorInstance->actorBehaviour->physics.velocity[Z]=vz;
		//this->actorInstance->actorBehaviour->physics.speed = bmVector2DLen(this->actorInstance->actorBehaviour->physics.velocity);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::respondToMessage
	Purpose 	: respond to a message
	Parameters 	: message string to which to respond
	Returns 	: true if the message was recognised and handled, else false
	Info 		: 
*/
bool ZBBAT::respondToMessage(const char* const message)
{
	// PP: respond to stomp message (start a stomp effect)
	if(stricmp(message, ZBBATMESSAGE_STOMP)==0)
	{
		TBVector				zeroVec={0.0f,0.0f,0.0f,0.0f};

		// PP: quake the ball, if it can hear this bat
		if(ZooBoss_game.ball.hear(*this))
		{
			ZooBoss_game.ball.quake();
		}

		// PP: STOP GOSSAMER FROM SLIDING
		this->setXZVelocity(0.0f, 0.0f);

		// PP: MAKE A SHOCKWAVE EFFECT
		Shockwave_create(this->pos(),
							ZBSHOCKWAVE_MAXRADIUS,
							ZBSHOCKWAVE_STARTSPEED,
							ZBSHOCKWAVE_ENDSPEED,
							ZBSHOCKWAVE_NUMRINGS,
							ZBSHOCKWAVE_INTERVAL,
							ZBSHOCKWAVE_STARTWIDTH,
							ZBSHOCKWAVE_ENDWIDTH,
							&ZooBoss_shockwaveColours[0]);

		// PP: MAKE A BIG BOOM NOISE
		// PP: PlaySample("explo_gen_01c.wav",255, this->pos());
		this->playSound("gosstomp.wav", AIUSOUND_POSITION);

		// PP: KICK UP SOME DUST
		CreateImpactClouds(this->pos(), zeroVec, NULL, METERTOUNIT(0.2f), 2.0f, 0.0f, SMOKE_BIGFALL);

		// PP: SHAKE THE CAMERA
/*		this->cam->quakeOffsetLevel = 0.035f;
		this->cam->tiltOffsetLevel = RAD(1.35f);
		this->cam->flags |= CAMERA_QUAKEFADE;*/

		return true;
	}

	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::scare
	Purpose 	: do the thing that scares (deflects) the ball (elephant)
	Parameters 	: 
	Returns 	: 
	Info 		: Taz shouts at the ball, Gossamer either shouts or causes an earthquake
*/
void ZBBAT::scare(void)
{
	if(this->type == ZBBATTYPE_TAZ)
	{
		// PP: Stop Taz from sliding
		this->setXZVelocity(0.0f, 0.0f);

		PlayAnimationByName(actorInstance,"scare1",1,0,0,0.2f,ATTACK);
	/*	if(characterSounds.taz == -1)
		{
			CallbackWhenSampleFinished((characterSounds.taz=PlaySample("taz_voc_29b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
		}*/

	/*	cameraInfo->quakeOffsetLevel = 0.02f;
		cameraInfo->tiltOffsetLevel = RAD(1.0f);
		cameraInfo->flags |= CAMERA_QUAKEFADE;*/
	}
	else if(this->type == ZBBATTYPE_GOSSAMER)
	{
		// PP: randomly chosen scare

		if(this->scareType == SCARE_STOMP)
		{
			PlayAnimationByName(actorInstance,"stomp",1,0,0,0.2f,ATTACK);
		}
		else// PP: if(this->scareType == SCARE_SHOUT)
		{
			PlayAnimationByName(actorInstance,"scare2",1,0,0,0.2f,ATTACK);
		}
	}

	// PP: generate a new random offset for interception line for next ai scare
	this->ai.interceptLineOffset=bmRandfSigned();

	this->actorInstance->actorStatus->lastState = this->actorInstance->actorStatus->currentState;
	this->actorInstance->actorStatus->currentState = STATE_SCARE;
}



/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::AIupdate
	Purpose 	: update the bat according to its AI
	Parameters 	: ref to the ball
	Returns 	: 
	Info 		: 
*/

#define LINE_LENGTH									METERTOUNIT(100)
													
#define COURT_MARGIN								METERTOUNIT(0.0f)		// PP: bit dodge
													
#define ZBBATAI_MIN_MOVE_DIST						METERTOUNIT(0.2f)		// PP: Distance above which the bat AI will bother moving the bat to it's destination position

#define ZBBATAI_DESTPOS_ADJUST						METERTOUNIT(-2.0f)		// PP: distance back from the predicted intersect point, the the bat moves in order to get properly in fron tof the ball and scare it forward

void ZBBAT::AIupdate(/*ZOOBOSSGAME* const game*/ZBBALL& ball)
{
	TBVector2D			distVec;							// PP: general-purpose 2D distamce vector
	float				dist;								// PP: general-purpose distance
	//ZBBALL*				ball=&(game->ball);					// PP: keep a pointer to the ball, 'cos I'm lazy like that
	bool				printLatch=false;
	bool				protectedWall=false;

	if((ball.flags&ZBBALLFLAG_NEWLYENTEREDCONTACTCOURT)||((ball.flags & ZBBALLFLAG_NEWSTATE) && (ball.flags & ZBBALLFLAG_NEWDIRECTION)))
	{
		// PP: TEST
		//PlaySample("tazjump.wav");

		//if(ball.state == ZBBALLSTATE_RUNNING)// PP: path prediction is so good now that I don't have to update after every turn, so nyer
		{
			numPathVerts=0;

			// PP: position the intercept line at a random x position (towards/away from opponent) near the x centre of this bat's court half...

			// PP: maximum court-lengthward offset of the bat ai's intercept line
#define ZBBATAI_MAX_INTERCEPT_LINE_OFFSET		METERTOUNIT(1)		// PP: was 4m (too far)

			this->ai.interceptLine.start[X]=this->court.midX()-this->ai.interceptLineOffset*ZBBATAI_MAX_INTERCEPT_LINE_OFFSET;
		
#define ZBBATAI_INTERCEPT_LINE_EDGE_MARGIN	100.0f

			// PP: drag the intercept line across from the wall on the bat's left to the wall on the bat's right
			this->ai.interceptLine.start[Y]=ball.contactCourt.top+ZBBATAI_INTERCEPT_LINE_EDGE_MARGIN;// PP: was court
			this->ai.interceptLine.end[X]=this->ai.interceptLine.start[X];
			this->ai.interceptLine.end[Y]=ball.contactCourt.bottom-ZBBATAI_INTERCEPT_LINE_EDGE_MARGIN;// PP: was court


			int					numPathLineSegments=0;			// PP: number of path line segments tested
			float				segmentYang;					// PP: y angle of ball path line segment	
				
			int					i;								// PP: loop counter - you know the way
			bool				hitBackWall=false;				// PP: ball will have hit our back wall
			const float*		wallPtr;						// PP: pointer to wall within ball's court, being tested
			TBCollisionLine2D	pathCollisionLine;				// PP: path segment for the ball
			TBCollisionLine2D	wallCollisionLine;				// PP: one of the court walls with which to check for collision
			TBCollisionLine2D	collisionLine2;
			bool				useCollisionLine2=false;
			TBCollisionInfo2D	collisionInfo;					// PP: 2D collision info struct for Babel collision tests				

			int					lastWall= -1;					// PP: last wall hit by a line segment
			float				wallAng;						// PP: wall angle

			// PP: does the new direction face the bat?  if so we will have a chance to scare it again if it gets close enough
			if(FindShortestAngle(ball.getYang(), Aabs(this->normalYang+PI)) < HALFPI)
			{
				this->ai.doneScare=false;

				// PP: if this is Gossamer, randomly choose between scares...
				if(this->type == ZBBATTYPE_GOSSAMER)
				{
					

					// PP: new test - shout if ball's slow, stomp if it's fast
					// PP: REMOUT: i'll try the other one again			if(ZooBoss_game.ball.speedFraction < ZBBALL_SPEED_SKIDHALT)
						
					if(bmRand()&1)
					{
						this->scareType=SCARE_SHOUT;
					}
					else
					{
						this->scareType=SCARE_STOMP;
					}
				}
			}

			// PP: ================= CALCULATE THE PATH OF THE BALL UNTIL THE POINT WHERE THAT PATH HITS OUR BACK WALL ======================

			// PP: clear the time we expect the ball to take to reach our destination point
			this->ai.eta=0.0f;

			// PP: start projecting the path from the position of the ball...
			pathCollisionLine.start[X]=ball.pos()[X];
			pathCollisionLine.start[Y]=ball.pos()[Z];

			// PP: ...and in the Y angle of the ball
			segmentYang=ball.getYang();

			// PP: set up collision info for tests
			collisionInfo.flags=BMCOLLINFO2D_RETURNPOSITION;

			if(ZooBoss_debug)
			{
				// PP: add the start point of the first line segment to the list of verts to be drawn in debug mode
				BDVERTEX_SETXYZW(&pathVerts[numPathVerts], pathCollisionLine.start[X], ZB_COURT_HEIGHT, pathCollisionLine.start[Y], 0.0f);
				BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 255, 255, 255, 255);
				++numPathVerts;
			}
			
			// PP: this loop bounces the 'path' line around the court till it hits our back wall
			do
			{
				// PP: set the end of the collision line, based on the start of the line and the y angle...
				pathCollisionLine.end[X]=pathCollisionLine.start[X]+(LINE_LENGTH*bmSin(TWOPI-segmentYang+PI));
				pathCollisionLine.end[Y]=pathCollisionLine.start[Y]+(LINE_LENGTH*bmCos(TWOPI-segmentYang+PI));

				// PP: see if we can move onto the path line segment...

				// PP: does the intercept line hit the path line segment?
				if(bmCollideLineLine2D(&pathCollisionLine, &this->ai.interceptLine, &collisionInfo) || (useCollisionLine2 && bmCollideLineLine2D(&collisionLine2, &this->ai.interceptLine, &collisionInfo)))
				{
					// PP: if so, the point we want to run to is the interception point
					// PP: ****************** TODO: MOVE THE DESTINATION POINT BACK A BIT? *******************
					bmVector2DCopy(this->ai.ballPosOnScare, collisionInfo.position);

					if(ZooBoss_debug)
					{
						// PP: add the new end point of this, now the last, line segment to the list of verts to be drawn in debug mode
						BDVERTEX_SETXYZW(&pathVerts[numPathVerts], this->ai.ballPosOnScare[X], ZB_COURT_HEIGHT, this->ai.ballPosOnScare[Y], 0.0f);
						BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 0, 255, 0, 128);

						numPathVerts++;
					}

/* PP: REMOUT: NEW TEST - I'm not sure this is really needed; might just be causing more problems than it solves

					// PP: move the dest pos back a bit - we want to scare the ball forward...
					TBVector2D		adjust;

					adjust[X]=ZBBATAI_DESTPOS_ADJUST *bmSin(this->normalYang);
					adjust[Y]=ZBBATAI_DESTPOS_ADJUST *bmCos(this->normalYang);

					bmVector2DAdd(this->ai.destPos, collisionInfo.position, adjust);*/

					// PP: *2D*!!! DUH!
					bmVector2DCopy(this->ai.destPos, collisionInfo.position);


					// PP: SHIFT THE DESTPOS LEFT OR RIGHT A BIT - THIS CAUSES THE BALL TO BE DEFLECTED AT MORE INTERESTING ANGLES...

/* PP: REMOUT: NEW TEST - I'm not sure this is really needed; might just be causing more problems than it solves

#define ZBBATAI_MAX_DESTPOS_SIDEWARDS_OFFSET		METERTOUNIT(3.0f)

					this->ai.destPos[Y]+=(-ZBBATAI_MAX_DESTPOS_SIDEWARDS_OFFSET*0.5f)+((bmRand()/RANDMAX)*ZBBATAI_MAX_DESTPOS_SIDEWARDS_OFFSET);
*/
					// PP: add the path line segment's time to ETA (time the bat expects the ball to reach its destination point)...
					distVec[X]=this->ai.ballPosOnScare[X]-ball.pos()[X];
					distVec[Y]=this->ai.ballPosOnScare[Y]-ball.pos()[Z];// PP: this was remmed out, dunno why
					dist=bmVector2DLen(distVec);

					if(ball.speed != 0.0f)
					{
						this->ai.eta+=dist/ball.speed;
					}

					// PP: break out of this do..while loop that projects the ball's path around the court;
					break;
				}


				// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~
				// PP: SO THEN,  WE DIDN'T FIND A SUITABLE POINT ON THE PATH LINE SEGMENT TO MOVE TOWARDS - DO COLLISION TESTS WITH THE COURT WALLS INSTEAD...
				// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~

				// PP: check for collision of the path line segment will all four walls...
				for(i=0; i<4; i++)
				{
					if(i == lastWall) continue;// PP: ...apart from the wall from which it originates

					useCollisionLine2=false;

					switch(i)
					{
						case 0:
						{
							// PP: TOP WALL

							protectedWall=false;

							wallAng=PI+HALFPI;

							wallPtr= &ball.court.top;

							wallCollisionLine.start[X]=ball.contactCourt.left;
							wallCollisionLine.start[Y]=ball.contactCourt.top;

							wallCollisionLine.end[X]=ball.contactCourt.right;
							wallCollisionLine.end[Y]=ball.contactCourt.top;
						}
						break;
						
						case 1:
						{
							// PP: BOTTOM WALL

							protectedWall=false;

							wallAng=HALFPI;

							wallPtr= &ball.court.bottom;

							wallCollisionLine.start[X]=ball.contactCourt.left;
							wallCollisionLine.start[Y]=ball.contactCourt.bottom;

							wallCollisionLine.end[X]=ball.contactCourt.right;
							wallCollisionLine.end[Y]=ball.contactCourt.bottom;
						}
						break;

						case 2:
						{
							// PP: RIGHT WALL

							protectedWall=true;

							wallAng=PI;

							wallPtr= &ball.court.right;

							wallCollisionLine.start[X]=ball.contactCourt.right;
							wallCollisionLine.start[Y]=ball.contactCourt.top;

							wallCollisionLine.end[X]=ball.contactCourt.right;
							wallCollisionLine.end[Y]=ball.contactCourt.bottom;
						}
						break;

						case 3:
						{
							// PP: LEFT WALL

							protectedWall=true;

							wallAng=0.0f;

							wallPtr= &ball.court.left;

							wallCollisionLine.start[X]=ball.contactCourt.left;
							wallCollisionLine.start[Y]=ball.contactCourt.top;

							wallCollisionLine.end[X]=ball.contactCourt.left;
							wallCollisionLine.end[Y]=ball.contactCourt.bottom;
						}
						break;
					}

					// PP: the path line has hit a wall
					if(bmCollideLineLine2D(&pathCollisionLine, &wallCollisionLine, &collisionInfo))
					{
						// PP: add the path line segment's time to ETA

						/*distVec[X]=collisionInfo.position[X]-ball.pos()[X];
						distVec[Y]=collisionInfo.position[Y]-ball.pos()[Z];*/

						distVec[X]=collisionInfo.position[X]-pathCollisionLine.start[X];
						distVec[Y]=collisionInfo.position[Y]-pathCollisionLine.start[Y];
						
						dist=bmVector2DLen(distVec);
						this->ai.eta+=dist/ball.speed;

						// PP: mark the wall that was hit as such, so's the next line segment doesn't try to test for collision with it (the next line segment will originate from the face of the wall that was hit wall)
						lastWall=i;

						// PP: if the wall that was hit is our back wall (that we're trying to protect)...
						if(wallPtr == this->wall)
						{
							// PP: ...um, remember that it has
							hitBackWall=true;
						}

						if(ZooBoss_debug)
						{
							// PP: add the end point of this line segment to the list of verts to be drawn in debug mode
							BDVERTEX_SETXYZW(&pathVerts[numPathVerts], collisionInfo.position[X], ZB_COURT_HEIGHT, collisionInfo.position[Y], 0.0f);

							// PP: colour it red if it hit our back wall
							if(hitBackWall)
							{
								BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 255, 0, 0, 255);
							}
							else
							{
								BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 128, 128, 128, 128);
							}

							numPathVerts++;
						}


						// PP: set the new collision line's start point

						if(protectedWall)
						{
							segmentYang=Aabs(bmATan2(ball.court.midY()-collisionInfo.position[Y], ball.court.midX()-collisionInfo.position[X])-HALFPI+PI);
							bmVector2DCopy(pathCollisionLine.start, collisionInfo.position);
						}
						else
						{
							float			diameter;
							float			theta;
							float			turnSpeed;
							float			cordLength;
							float			cordAngle;
							TBVector2D		offset;
													
							turnSpeed=ball.getTurnSpeed();

							// PP: calculate circumference of turning circle
							// PP: (movement speed * time for full rotation)/PI
							diameter=(ball.speed*(TWOPI/turnSpeed))/PI;
								
							// PP: get angle of intersect with the wall
							theta=FindShortestAngleSigned(wallAng, Aabs(segmentYang+HALFPI));

							if(!printLatch)
							{
								//oBoss_debugBox->sprintf("theta: %5.2f\n", theta);
								printLatch=true;
							}

							// PP: temp test
							//theta=0.0f;

							// PP: calculate length of cord across turning circle
							cordLength=diameter*(bmCos(theta));

							if(cordLength < 0.01f)
							{
								int g=0;
							}

							// PP: offset start of next segment by the cord, which is parallel to the wall
							cordAngle=Aabs(PI+wallAng);

							if(theta<0.0f)
							{
								cordAngle=TWOPI-cordAngle;
							}

							offset[X]=cordLength*bmSin(cordAngle);
							offset[Y]=cordLength*bmCos(cordAngle);

							//ZooBoss_debugBox->sprintf("radius: %5.2f\n", UNITTOMETER(radius));

							bmVector2DCopy(collisionLine2.start, collisionInfo.position);

							bmVector2DAdd(pathCollisionLine.start, collisionInfo.position, offset);

							bmVector2DCopy(collisionLine2.end, pathCollisionLine.start);

							useCollisionLine2=true;

							if(ZooBoss_debug)
							{
								// PP: add the start point of this line segment to the list of verts to be drawn in debug mode
								BDVERTEX_SETXYZW(&pathVerts[numPathVerts], collisionLine2.start[X], ZB_COURT_HEIGHT, collisionLine2.start[Y], 0.0f);
								BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 0, 0, 255, 128);

								numPathVerts++;

								// PP: add the start point of this line segment to the list of verts to be drawn in debug mode
								BDVERTEX_SETXYZW(&pathVerts[numPathVerts], collisionLine2.end[X], ZB_COURT_HEIGHT, collisionLine2.end[Y], 0.0f);
								BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 0, 0, 255, 128);

								numPathVerts++;
							}

							// PP: add the turn's time onto the eta
							this->ai.eta+=((bmFAbs(theta)/PI)*(TWOPI/turnSpeed));

							segmentYang=Aabs(wallAng+(wallAng-segmentYang));
						}

						if(ZooBoss_debug)
						{
							// PP: add the start point of this line segment to the list of verts to be drawn in debug mode
							BDVERTEX_SETXYZW(&pathVerts[numPathVerts], pathCollisionLine.start[X], ZB_COURT_HEIGHT, pathCollisionLine.start[Y], 0.0f);
							BDVERTEX_SETRGBA(&pathVerts[numPathVerts], 128, 128, 128, 128);

							numPathVerts++;
						}
						
						break;// PP: break out of the for loop that tests each of the walls, 'cos we hit one, remember?
					}
				}

				// PP: another ball path line segment has been tested
				++numPathLineSegments;

				if(numPathLineSegments>=ZBBATAI_MAX_BALL_PATH_LINE_SEGMENTS)
				{
					bkPrintf("schtopp!\n");
				}

			}while((!hitBackWall)&&(numPathLineSegments<ZBBATAI_MAX_BALL_PATH_LINE_SEGMENTS));// PP: enforce the limit on the number of ball path line segments we want to test, otherwise things would get out of hand
		}
	}


	// PP: set bat's movement towards the intersect point...
	float			angleOfMovement;

	distVec[X]=this->ai.destPos[X]-this->pos()[X];
	distVec[Y]=this->ai.destPos[Y]-this->pos()[Z];

	dist=bmVector2DLen(distVec);

	if(dist > ZBBATAI_MIN_MOVE_DIST)
	{
		if(this->allowMove)
		{
			this->overallSpeedFraction=this->ai.runSpeedFactor;// PP: 1.0f;

			angleOfMovement = Aabs(PI-bmATan2(distVec[X], distVec[Y]));

			this->moveYangRelative=FindShortestAngleSigned(angleOfMovement, this->getYang());

			// PP: get forward movement aligned to character
			this->speedFractions[Y]=this->overallSpeedFraction *bmCos(this->moveYangRelative)*this->runSpeed;

			// PP: get sideward movement aligned to character
			this->speedFractions[X]=this->overallSpeedFraction *bmSin(this->moveYangRelative)*this->runSpeed;
		}
		else
		{
			this->overallSpeedFraction=0.0f;
			this->speedFractions[X]=0.0f;
			this->speedFractions[Y]=0.0f;
		}
	}
	else
	{
		// PP: we're close enough - stop

		this->speedFractions[X]=0.0f;
		this->speedFractions[Y]=0.0f;
		this->overallSpeedFraction=0.0f;
		this->moveYangRelative=0.0f;
	}


	if(!this->ai.doneScare)
	{
		// PP: if the ball's close enough time-wise, scare it
// PP: 		if(this->ai.eta < this->ai.timeToScare)// ZBBATAI_TIME_TO_SCARE)
		{
			// PP: if we're facing it that is
// PP: 			if(FindShortestAngle(ball.getYang(), Aabs(this->normalYang+PI)) < HALFPI)
			{
				// PP: if the ball CAN HEAR US! duh!
				if(ball.hear(*this))
				{
					//bkPrintf("ai scaring ball - ball.getYang %5.2f, Aabs(this->normalYang+PI) %5.2f, FindShortestAngle %5.2f\n", ball.getYang(), Aabs(this->normalYang+PI), FindShortestAngle(ball.getYang(), Aabs(this->normalYang+PI)));

					this->scare();
					this->ai.doneScare=true;
				}
			}
		}
	}

	// PP: tick down the eta
	this->ai.eta-=fTime;

//	ZooBoss_debugBox->sprintf("eta %5.2f", this->ai.eta);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::humanUpdate
	Purpose 	: update the bat according to player input
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBAT::humanUpdate(void)
{
	float					angleOfMovement;				// PP: angle of the bat's movement
	PADCONTROLSTATUS*		controller=this->player->controller;
	CAMERAINFO*				cameraInfo=this->player->camera;			// PP: copy of the bat's camera info, for convenience
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;

	if((controller != NULL)&&(gameStatus.controlSuspend==false)) // TP: use control suspend for end of level stats
	{
		if(this->allowMove)
		{
			switch(actorStatus->currentState)
			{
			case STATE_MOVE:
				{
					// PP: MILESTONE BODGE
					// PP: this->setYang(this->normalYang);

					angleOfMovement = Aabs(PI-ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1));

					this->overallSpeedFraction = controller->mag1;

					this->moveYangRelative=FindShortestAngleSigned(angleOfMovement, this->getYang());

					// PP: get forward movement aligned to character
					speedFractions[Y]=this->overallSpeedFraction *bmCos(this->moveYangRelative)*this->runSpeed;

					// PP: get sideward movement aligned to character
					speedFractions[X]=this->overallSpeedFraction *bmSin(this->moveYangRelative)*this->runSpeed;

					// check for scare move
					if (controller->squareDebounceChannel->value)
					{
						this->scare();
					}

					// PP: TEST: EAT/ SPIT

#ifdef ZB_JUMP
					// check for player pressing jump
					if(CheckForJump(player)) break;
#endif// PP: def ZB_JUMP

					if(this->type == ZBBATTYPE_TAZ && gameStatus.multiplayer.versusCPU)
					{	// JW: Don't let either character spin in 2 player// PP: because it gives Taz an advantage
						// check for spin mode
						if(CheckForSpin(player)) break;
					}

#ifdef ZB_BITE
					// check for bite attack
					if (controller->triangleDebounceChannel->value)
					{
						if (actorInstance->characterInfo->inMouth)
						{
							PlayAnimationByName(this->actorInstance,"attack1",1,0,0,0.2f,ATTACK);
							TazSpit(this->actorInstance);
							this->actorInstance->actorStatus->lastState = this->actorInstance->actorStatus->currentState;
							this->actorInstance->actorStatus->currentState = STATE_SPIT;
							//-----------------STATE CHANGE-----------------
						}
						else
						{
							PlayAnimationByName(this->actorInstance,"attack1",1,0,0,0.2f,ATTACK);
							this->actorInstance->actorStatus->lastState = this->actorInstance->actorStatus->currentState;
							this->actorInstance->actorStatus->currentState = STATE_BITE;
							//-----------------STATE CHANGE-----------------
						}

						break;
					}
#endif// PP: def ZB_BITE
				}
				break;

			case STATE_JUMP:
				{
					HandleJumpState(this->player, true, false);
				}
				break;

			case STATE_SPINUP:
				{
					HandleSpinUpState(this->player, true);
				}
				break;

			case STATE_SPIN:
				{
					HandleSpinState(this->player, true);
				}
				break;

			case STATE_SPINDOWN:
				{
					// PP: MILESTONE BODGE
					this->setYang(this->normalYang);

					HandleSpinDownState(this->player);
				}
				break;

			case STATE_SQUASHED:
				{
					HandleSquashedState(this->player);
				}
				break;


			}// PP: end switch
		}
		else
		{
			this->overallSpeedFraction=0.0f;
			this->speedFractions[X]=0.0f;
			this->speedFractions[Y]=0.0f;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::setState
	Purpose 	: set the bat's state
	Parameters 	: state to set (ZBBATSTATE_...)
	Returns 	: 
	Info 		: NOTE TO SELF: always use setState methods! You know it makes sense!
*/	
void ZBBAT::setState(const ZBBatState stateIn)
{
	switch(stateIn)
	{
	case ZBBATSTATE_WINNING:
		{
			if(this->type == ZBBATTYPE_GOSSAMER)
			{
				// PP: gossamer has scored a point - sam celebrates
				zbsam.win();
			}

			// PP: do this first cos celebrate checks for max score
			this->increaseScore();	// PP: this bat has won a goal	

			this->celebrate(ZooBoss_game.bats[ZooBoss_game.loser]);

			if(this->getScore() >= ZooBoss_game.getMaxScore())
			{
				this->setState(ZBBATSTATE_WON);
				break;
			}

			this->allowMove=false;

			RequestStateChange(this->actorInstance, STATE_MOVE);

			if((ZooBoss_game.twoPlayer)||(this == &(ZooBoss_game.bats[0])))
			{
				ZooBoss_game.messageBook.open("GOAL");
				ZooBoss_game.messageBook.close(ZB_DURATION_STANDARDMESSAGE);
			}
		}
		break;

	case ZBBATSTATE_LOSING:
		{
			if(this->type == ZBBATTYPE_GOSSAMER)
			{
				// PP: gossamer has lost a goal - sam isn't happy
				zbsam.lose();
			}

			this->allowMove=false;

			RequestStateChange(this->actorInstance, STATE_MOVE);

			if(ZooBoss_game.twoPlayer)
			{
				// PP: into one-screen for this bit
				setNumScreens(1);
			}

			if((!ZooBoss_game.twoPlayer)&&(this == &(ZooBoss_game.bats[0])))
			{
				ZooBoss_game.messageBook.open("LOSEPOINT");
				ZooBoss_game.messageBook.close(ZB_DURATION_STANDARDMESSAGE);
			}

			this->sulk(ZooBoss_game.bats[ZooBoss_game.winner]);
		}
		break;

	case ZBBATSTATE_WON:
		{
			this->allowMove=false;

			RequestStateChange(this->actorInstance, STATE_MOVE);

			if(ZooBoss_game.twoPlayer)// PP: in 1 player this is triggered by cameras
			{
				ZooBoss_game.bats[ZooBoss_game.loser].setState(ZBBATSTATE_LOST);
			}
		}
		break;

	case ZBBATSTATE_LOST:
		{
			this->allowMove=false;

			RequestStateChange(this->actorInstance, STATE_MOVE);

			this->sulk(ZooBoss_game.bats[ZooBoss_game.winner]);

			ZooBoss_game.setState(BGSTATE_OUTRO);

			//ZooBoss_game.bats[ZooBoss_game.winner].setState(ZBBATSTATE_WON);
		}
		break;

	case ZBBATSTATE_PLAYING:
		{
			this->allowMove=true;

			// PP: NEW - cancel any anims in progress
			FlushAnimationQueue(this->actorInstance);
		}
		break;
	}

	this->state = stateIn;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::update
	Purpose 	: update the bat
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBAT::update(void)
{
	ACTOR_STATUS			*actorStatus=NULL;				// PP: copy of the bat's actor instance status, for convenience
	PADCONTROLSTATUS		*controller=NULL;				// PP: copy of the bat's controller, for convenience

	FADE					*fade = NULL;					// PP: copy of the bat's fade info, for convenience


	TBActorAnimSegment*		runSegment;						// PP: forward/backward anim to use
	TBActorAnimSegment*		strafeSegment;					// PP: sideward anim to use

	bool					backwards;						// PP: are we running backwards?  This could be dangerous, especially if we're carrying scissors or going down stairs

	unsigned int			idleFrame;						// PP: frame to use from idle anim
	unsigned int			runFrame;						// PP: frame to use from forward/backward anim
	unsigned int			strafeFrame;					// PP: frame to use from sideward anim

	ACTORINSTANCEUSER::update();	// PP: this updates animation-changed flag

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// PP: TEMP FOR TESTING - set up anims again in case we've changed character...
/*	this->anims.run=baFindAnimSegment(&(this->actorInstance->actorInstance), "run");
	this->anims.runSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runstop");

	if(this->type == ZBBATTYPE_TAZ)
	{
		this->anims.runBack=baFindAnimSegment(&(this->actorInstance->actorInstance), "runback");
		this->anims.runBackSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runbackstop");
	}
	else
	{
		this->anims.runBack=this->anims.run;
		this->anims.runBackSkid=this->anims.runSkid;
	}

	this->anims.strafeRight=baFindAnimSegment(&(this->actorInstance->actorInstance), "runright");
	this->anims.strafeRightSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runrightstop");

	this->anims.strafeLeft=baFindAnimSegment(&(this->actorInstance->actorInstance), "runleft");
	this->anims.strafeLeftSkid=baFindAnimSegment(&(this->actorInstance->actorInstance), "runleftstop");

	this->anims.idle=baFindAnimSegment(&(this->actorInstance->actorInstance), "idle1");*/

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	// PP: UPDATE BOOK
// PP: REMOUT: Now handled by the Book List			this->book.update();

	//if(this->book.

	// PP: UPDATE CONTROL, BE IT AI OR HUMAN....

	// PP: ai-update only if the bat has no player
	if(this->player == NULL)
	{
		this->AIupdate(ZooBoss_game.ball);
	}
	else
	{
		this->humanUpdate();
	}


/*
	if(this->pos()[X] > testCourt.right)
	{
		this->speedFractions[Y]=0.0f;
	}*/

//his->court.removeMargin(METERTOUNIT(1)).contain(this->ai.destPos);


	// PP: keep pointers to useful data...

	actorStatus = this->actorInstance->actorStatus;

	if(this->player != NULL)
	{
		//controller=this->player->controller;
		//cameraInfo=this->player->camera;
		fade= &(this->player->camera->fade);
	}

	// PP: restrict bat to its court
	this->court.contain(this->pos()/*, this->radius*/);

	// PP: REMOUT: TO HELL WITH IT ALL

	// PP: reposition taz if he's fallen off the world
	if(actorStatus != NULL)
	{
		if(fade != NULL)
		{
			if ((actorStatus->currentState!=STATE_DEBUGMOVE)&&(actorStatus->currentState!=STATE_DEAD)
				&&(actorStatus->stateRequest!=STATE_DEAD)&&(fade->GetFadeState()==FADESTATE_OFF))
			{
				if (this->pos()[Y] < (worldBoundingBox.ymin-METERTOUNIT(2.0f)))
				{
					ProcessCharacterDeath(actorInstance);
				}
			}
		}
	}



/*	if(controller != NULL)
	{
		// PP: TEMP TEST - control bat's Y angle with the circle button
		if(controller->circleChannel->value != 0)
		{
			this->setYang(this->getYang()+0.1f);

			bkPrintf("ang %d", this->getYang());
		}

		if(controller->triangleDebounceChannel->value != 0)
		{
			baFlushNodeQueue(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance, TRUE);
		}
	}*/

	switch(actorStatus->currentState)
	{
		case STATE_INIT:// PP: INIT state ain't used round these parts, Stranger.
		case STATE_RUNON:// PP: RUNON state ain't used round these parts, Stranger.
		{
			actorStatus->lastState = actorStatus->currentState;
			actorStatus->currentState = STATE_MOVE;
		}
		break;

		case STATE_MOVE:
		{
			if (RespondToStateExit(this->actorInstance)) 
			{
				RespondToStateExitPost(this->actorInstance,actorStatus);
				break;
			}

			// PP: calculate current idle frame
			this->idlePhase=(float)bmFMod(this->idlePhase+((1.0f/(float)(ANIMFRAMES(this->anims.idle))*60.0f/fps)), 1.0f);
			idleFrame=bmFloatToInt(this->anims.idle->endFrame-(this->idlePhase*ANIMFRAMES(this->anims.idle)*BTICKSPERFRAME));

			// PP: **************************************** CONSISTENCY WARNING **********************************************
			// PP: REMOUT: leave skidding for just now
			// PP: **************************************** CONSISTENCY WARNING **********************************************
/*
			if(CheckIfPlayingAnimation(actorInstance,"runleft")||CheckIfPlayingAnimation(actorInstance,"runright")
				||CheckIfPlayingAnimation(actorInstance,"run")||CheckIfPlayingAnimation(actorInstance,"runback"))
			{
				if(!RequestStateChange(actorInstance, STATE_SKIDSTOP))
				{
					bkPrintf("state change failed\n");
				}
				//-----------------STATE CHANGE-----------------
				break;
			}
*/
		

			// PP: update the run phase
			this->runPhase=(float)bmFMod(this->runPhase+((1/(float)(ANIMFRAMES(this->anims.run))*60/fps)), 1.0f);

			// PP: pose the bat according to its current point in its run & strafe...


			if(bmFAbs(this->moveYangRelative)<HALFPI)
			{
				backwards=false;
				runSegment=this->anims.run;
			}
			else
			{
				backwards=true;
				runSegment=this->anims.runBack;
			}

			// PP: MAKE SURE BAT ISN'T TRYING TO RUN FARTHER THAN THE COURT WALLS PERMIT...

			// PP: disallow movement and animation in blocked direction...

			RECTANGLE	testCourt;

			testCourt=this->court.removeMargin(METERTOUNIT(0.1f));

			// PP: No, I don't like the look of this either.  Like I say, I made a bit of a pig's arse of this boss game.  Ye live & learn, eh? ...

			if(this->type == ZBBATTYPE_TAZ)
			{

				// PP: forward limit

				if(this->pos()[X] < testCourt.left)
				{
					if(this->speedFractions[Y] > 0.0f)
					{
						this->speedFractions[Y] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[X])/this->runSpeed;

						if(this->moveYangRelative>0.0f)
						{
							this->moveYangRelative = HALFPI;
						}
						else
						{
							this->moveYangRelative = -HALFPI;
						}
					}
				}

				// PP: backward limit

				if(this->pos()[X] > testCourt.right)
				{
					if(this->speedFractions[Y] < 0.0f)
					{
						this->speedFractions[Y] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[X])/this->runSpeed;

						if(this->moveYangRelative>0.0f)
						{
							this->moveYangRelative = HALFPI;
						}
						else
						{
							this->moveYangRelative = -HALFPI;
						}
					}
				}

				// PP: left limit

				if(this->pos()[Z] < testCourt.bottom)
				{
					if(this->speedFractions[X] > 0.0f)
					{
						this->speedFractions[X] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[Y])/this->runSpeed;

						this->overallSpeedFraction=this->speedFractions[Y]/this->runSpeed;

						if(backwards)
						{
							this->moveYangRelative = PI;
						}
						else
						{
							this->moveYangRelative = 0.0f;
						}
					}
				}

				// PP: right range

				if(this->pos()[Z] > testCourt.top)
				{
					if(this->speedFractions[X] < 0.0f)
					{
						this->speedFractions[X] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[Y])/this->runSpeed;

						if(backwards)
						{
							this->moveYangRelative = PI;
						}
						else
						{
							this->moveYangRelative = 0.0f;
						}
					}
				}
			}
			else if(this->type == ZBBATTYPE_GOSSAMER)
			{
				// PP: forward limit

				if(this->pos()[X] < testCourt.left)
				{
					if(this->speedFractions[Y] < 0.0f)
					{
						this->speedFractions[Y] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[X])/this->runSpeed;

						if(this->moveYangRelative>0.0f)
						{
							this->moveYangRelative = HALFPI;
						}
						else
						{
							this->moveYangRelative = -HALFPI;
						}
					}
				}

				// PP: backward limit

				if(this->pos()[X] > testCourt.right)
				{
					if(this->speedFractions[Y] > 0.0f)
					{
						this->speedFractions[Y] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[X])/this->runSpeed;

						if(this->moveYangRelative>0.0f)
						{
							this->moveYangRelative = HALFPI;
						}
						else
						{
							this->moveYangRelative = -HALFPI;
						}
					}
				}

				// PP: left limit

				if(this->pos()[Z] < testCourt.bottom)
				{
					if(this->speedFractions[X] < 0.0f)
					{
						this->speedFractions[X] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[Y])/this->runSpeed;

						if(backwards)
						{
							this->moveYangRelative = PI;
						}
						else
						{
							this->moveYangRelative = 0.0f;
						}
					}
				}

				// PP: right range

				if(this->pos()[Z] > testCourt.top)
				{
					if(this->speedFractions[X] > 0.0f)
					{
						this->speedFractions[X] = 0.0f;

						this->overallSpeedFraction=bmFAbs(this->speedFractions[Y])/this->runSpeed;

						if(backwards)
						{
							this->moveYangRelative = PI;
						}
						else
						{
							this->moveYangRelative = 0.0f;
						}
					}
				}
			}

	//		testCourt.contain(this->pos());

			// PP: ~~~~~~~~~~~~~~~~~

			if(this->moveYangRelative<0.0f)
			{
				if(backwards)
				{
					strafeSegment=this->anims.strafeLeft;
				}
				else
				{
					strafeSegment=this->anims.strafeRight;
				}
			}
			else
			{
				if(backwards)
				{
					strafeSegment=this->anims.strafeRight;
				}
				else
				{
					strafeSegment=this->anims.strafeLeft;
				}
			}

			// PP: calculate run frame & strafe frame...

			runFrame=bmFloatToInt(runSegment->startFrame+(this->runPhase*ANIMFRAMES(runSegment)*BTICKSPERFRAME));

			if(backwards)
			{	
				strafeFrame=bmFloatToInt(strafeSegment->endFrame-(this->runPhase*ANIMFRAMES(strafeSegment)*BTICKSPERFRAME));
			}
			else
			{
				strafeFrame=bmFloatToInt(strafeSegment->startFrame+(this->runPhase*ANIMFRAMES(strafeSegment)*BTICKSPERFRAME));
			}

			
			// PP: need this for is you get squished without losing a goal, eg. after a super-spin scare when the elephant lands and you've stopped spinning
// PP: REMOUT: TEMP TEST			FlushAnimationQueue(this->actorInstance);

			// PP: BLEND BETWEEN FORWARD/BACKWARD RUN AND LEFT/RIGHT RUN
			baSetNodeBlend(&(this->actorInstance->actorInstance),
							this->actorInstance->actorInstance.rootNodeInstance,
							runFrame, strafeFrame, MIN(bmFAbs(this->moveYangRelative), PI-bmFAbs(this->moveYangRelative))/HALFPI, 1);

			// PP: calculate speed
			this->speed=this->overallSpeedFraction*this->runSpeed;

			// PP: move the bat
			this->setXZVelocity((this->speedFractions[Y]*bmSin(PI-this->getYang()))+(this->speedFractions[X]*bmCos(this->getYang())),
								(this->speedFractions[Y]*bmCos(PI-this->getYang()))+(this->speedFractions[X]*bmSin(this->getYang())));

			// PP: OK, NOW BLEND WITH IDLE FRAME

			baSetNodeBlendTargetValue(&this->actorInstance->actorInstance, NULL, idleFrame, (1.0f-overallSpeedFraction), 1);

			baUpdateNodes(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance, 0.1f, NULL);// PP: 1.0f could be 0 i think

			/*if(controller != NULL)
			{
				// check for scare move
				if (controller->squareDebounceChannel->value)
				{
					this->scare();
					//-----------------STATE CHANGE-----------------

					break;
				}
			}*/

		}// PP: end case STATE_MOVE
		break;

		case STATE_SQUASHED:
		{
			if(RespondToStateExit(this->actorInstance))
			{
				// PP: REMOUT: don't take any health off for getting squished
				/*if(this->actorInstance->actorBehaviour != NULL)
				{
					ReduceCharactersHealth(actorInstance,1);
				}*/
				DestroyInjuryStars(this->actorInstance);
				RespondToStateExitPost(this->actorInstance,actorStatus);

				break;
			}
			if(!CheckIfPlayingAnimation(actorInstance, NULL))
			{
				// PP: REMOUT: don't take any health off for getting squished
				/*if(this->actorInstance->actorBehaviour != NULL)
				{
					ReduceCharactersHealth(actorInstance,1);
				}*/

				DestroyInjuryStars(actorInstance);
				//PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.1f,NONE);
				actorStatus->lastState = actorStatus->currentState;
				actorStatus->currentState = STATE_MOVE;

				// PP: flush the node queue to stop nasty things happening
				baFlushNodeQueue(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance, TRUE);

				//-----------------STATE CHANGE-----------------
				break;
			}
	
		}
		break;

		case STATE_RECOVER:
		{
			if (RespondToStateExit(this->actorInstance)) 
			{
				this->resetYang();

				this->actorInstance->actorAnimation->useMotionBone = FALSE;

				DestroyInjuryStars(this->actorInstance);
				RespondToStateExitPost(this->actorInstance,actorStatus);
				break;
			}

			if (ZBBAT_NO_CURRENT_ANIM)
			{
				this->resetYang();

				this->actorInstance->actorAnimation->useMotionBone = FALSE;

				DestroyInjuryStars(this->actorInstance);

				actorStatus->lastState = actorStatus->currentState;
				actorStatus->currentState = STATE_MOVE;
				//-----------------STATE CHANGE-----------------
				break;
			}
		}
		break;

		case STATE_DEAD:
		{
			if(!CheckIfPlayingAnimation(actorInstance, "idle2"))
			{
				ProcessCharacterDeath(actorInstance);
				actorStatus->lastState = actorStatus->currentState;
				actorStatus->currentState = STATE_MOVE;
			}
		}
		break;

		case STATE_SCARE:
		{
			if (RespondToStateExit(this->actorInstance)) 
			{
				RespondToStateExitPost(this->actorInstance,actorStatus);
				break;
			}

			// return to move when scare is finished
			if (ZBBAT_NO_CURRENT_ANIM)
			{
				actorStatus->lastState = actorStatus->currentState;
				actorStatus->currentState = STATE_MOVE;
				//-----------------STATE CHANGE-----------------
				break;
			}
			break;
		}
		break;

	case STATE_BITE:
		if (RespondToStateExit(this->actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		TazBite(actorInstance);	// check for the effects of a taz bite move

		// return to move when biteattack is finished
		if (!CheckIfPlayingAnimation(actorInstance,"attack1"))
		{
			if(actorInstance->characterInfo)
			{
				if(actorInstance->characterInfo->powerUpInfo.chilliPepper)
				{
					actorStatus->lastState = actorStatus->currentState;
					actorStatus->currentState = STATE_CHILLIPEPPER;
				}
				else
				{
					actorStatus->lastState = actorStatus->currentState;
					actorStatus->currentState = STATE_MOVE;
				}
				break;
			}
			else
			{
				actorStatus->lastState = actorStatus->currentState;
				actorStatus->currentState = STATE_MOVE;
			}
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;

	}// PP: end switch


#ifdef ZBBAT_HEADTRACK
	// PP: NEW TEST - turn to face target character

// PP: moveme
#define ZBBAT_HEADTRACK_ANGRANGE_X		TWOPI
#define ZBBAT_HEADTRACK_ANGRANGE_Y		TWOPI

/*
	if(this->characterToLookAt != NULL)
	{
		baNodeLookAt(
			&this->actorInstance->actorInstance,
			this->waistBone,
			this->characterToLookAt->pos(),
			-ZBBAT_HEADTRACK_ANGRANGE_X,
			ZBBAT_HEADTRACK_ANGRANGE_X,
			0.0f,
			-ZBBAT_HEADTRACK_ANGRANGE_Y,
			ZBBAT_HEADTRACK_ANGRANGE_Y,
			-(PI+HALFPI),// PP: y angle offset
			0.75f,// PP: turn speed
			BNODELOOKAT_PREP
		);
	}
	/*/

	// PP: TEST
	baPrepActorInstance(&(this->actorInstance->actorInstance), BACTORPREPFLAG_PREPNODES);

	if(this->characterToLookAt != NULL)
	{
		baNodeLookAtUnconstrained(
			&this->actorInstance->actorInstance,
			this->waistBone,
			characterToLookAt->pos(),//((this->pos()+ZVEC.rotateY(VEC(ZooBoss_game.ball.headPos).yAng(this->pos()))).setY(ZooBoss_game.ball.headPos[Y])),
			/*ZVEC.rotateX(RAD(20))*/ZVEC,
			1.0f,
			1.0f,
			0
		);
	}

	// PP: TEST
	//baPrepActorInstance(&(this->actorInstance->actorInstance), BACTORPREPFLAG_PREPNODES);

	//*/

#endif// PP: def ZBBAT_HEADTRACK

#ifdef ZB_PONGDUST

	// PP: spinning throws up a lot of dust

	// PP: frequency of dust particle emission during spin
#define ZBBAT_SPIN_DUST_FREQUENCY		40// PP: 20

	if(this->actorInstance->actorStatus->currentState == STATE_SPIN)
	{
		PongDust_create(this->pos(), 1, ZVEC, ZBBAT_SPIN_DUST_FREQUENCY);
	}

#endif// PP: def ZB_PONGDUST

	// PP: TEST - keep bats above court height
//	this->pos()[Y]=MAX(this->pos()[Y], ZB_COURT_HEIGHT);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::isDeflecting
	Purpose 	: find out if the bat is ready to deflect a ball
	Parameters 	: the ball against which to test for deflection
	Returns 	: 
	Info 		: 
*/
#define ZB_DEFLECTION_MAXSPEED_FRACTION		0.9f

bool ZBBAT::isDeflecting(const ZBBALL& ball)
{
#ifdef ZB_SUPERSPIN
	// PP: ball can be deflected by hitting it with a super-spin
	if(this->actorInstance->actorStatus->currentState == STATE_SPIN)
	{
		if(this->dist(ball) < ball.outerCollRadius)
		{
			this->scareType=SCARE_SUPERSPIN;

			// PP: set spinDeflection flag so's the ball can pick it up and trigger applause or whatever - maybe even a cheat??? :o)
			return(this->actorInstance->actorBehaviour->physics.speed > (MAX_SPIN_SPEED(this->actorInstance)*ZB_DEFLECTION_MAXSPEED_FRACTION));
		}
	}
#endif// PP: def ZB_SUPERSPIN

	this->scareType=SCARE_SHOUT;

	// PP: ball can be deflected by scaring it
	return(this->actorInstance->actorStatus->currentState == STATE_SCARE);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::getHit
	Purpose 	: get hit by a ball (when not prepared for it)
	Parameters 	: ball by which to get hit
	Returns 	: 
	Info 		: 
*/
void ZBBAT::getHit(const ZBBALL& ball)
{
	// PP: stop the bat sliding
	this->setXZVelocity(0.0f, 0.0f);

	if(this->type == ZBBATTYPE_TAZ)
	{
		// PP: FLATTEN TAZ

		RequestStateChange(this->actorInstance,STATE_SQUASHED);
		CreateInjuryStars(this->actorInstance);

		FlushAnimationQueue(this->actorInstance);

		if(bmRand()&1)
		{
			PlayAnimationByName(this->actorInstance,"recover22c",1.0f,0,0,0.1f,NONE);
			PlaySample("taz_gen_10a.wav",255);
		}
		else
		{
			PlayAnimationByName(this->actorInstance,"recover21b",1.0f,0,0,0.1f,NONE);
		}
	}
	else if(this->type == ZBBATTYPE_GOSSAMER)
	{
		// PP: FLATTEN GOSSAMER

		RequestStateChange(this->actorInstance,STATE_SQUASHED);
		CreateInjuryStars(this->actorInstance, METERTOUNIT(0.7f));

		FlushAnimationQueue(this->actorInstance);

		PlayAnimationByName(this->actorInstance,"flattened",1.0f,0,0,0.1f,NONE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::getKnocked
	Purpose 	: get knocked away by the elephant's side-belly when skidding to a halt,
					or by its rear hooves as it starts  running away
	Parameters 	: (opt/NULL) ball by which to get hit
	Returns 	: 
	Info 		: 
*/
void ZBBAT::getKnocked(const ZBBALL* const ball)
{
	float			ang;
	TBVector2D		dist;

	if(ball != NULL)
	{
		// PP: point the bat towards the ball...

		dist[X]=ball->pos()[X]-this->pos()[X];
		dist[Y]=ball->pos()[Z]-this->pos()[Z];

		ang=bmATan2(dist[Y], dist[X]);

		this->setYang(Aabs(ang+HALFPI));
	}

	// PP: play the knocked anim...

	FlushAnimationQueue(this->actorInstance);
	PlayAnimationBySegmentPointer(this->actorInstance, this->anims.knocked, 1.0f, 0, 0, 0.1f, NONE);

	this->actorInstance->actorAnimation->useMotionBone = TRUE;

	// PP: switch to recovery state

	RequestStateChange(this->actorInstance,STATE_RECOVER);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBAT::getProdded
	Purpose 	: get prodded by the elephant's tusks as it skids to a halt (its head is in a lowered position then)
	Parameters 	: ball by which to get hit
	Returns 	: 
	Info 		: 
*/
#define ZBBAT_MAX_PRODDED_KOIK_SPEED	METERTOUNIT(10)			// PP: maximum upwards speed, ie upwards speed when prodded by a full-speed elephant
#define ZBBAT_MIN_PRODDED_KOIK_SPEED	METERTOUNIT(1)			// PP: minimum upwards speed, ie upwards speed when prodded by a min-speed elephant

void ZBBAT::getProdded(const ZBBALL& ball)
{
	float			ang;
	TBVector2D		distVec;

	// PP: point the bat towards the ball...

	distVec[X]=ball.pos()[X]-this->pos()[X];
	distVec[Y]=ball.pos()[Z]-this->pos()[Z];

	ang=bmATan2(distVec[Y], distVec[X]);

	this->setYang(Aabs(ang+HALFPI));

	// PP: play the prodded anim...

	FlushAnimationQueue(this->actorInstance);
	PlayAnimationBySegmentPointer(this->actorInstance, this->anims.prodded, 1.0f, 0, 0, 0.1f, NONE);

	this->actorInstance->actorAnimation->useMotionBone = TRUE;

	// PP: TEST - set y velocity
	{
		float koikSpeed;		// PP: upwards speed of koik

		koikSpeed=METERTOUNIT(5);//ZBBAT_MIN_PRODDED_KOIK_SPEED+((ZBBAT_MAX_PRODDED_KOIK_SPEED-ZBBAT_MIN_PRODDED_KOIK_SPEED)*ball.speedFraction);

		CalculateSpeedBeforeJump(actorInstance);	// fill in current speed before jump
		actorInstance->actorInstance.position[Y] += 1.0f;	// bodge to avoid sticking to ground

		// zero vertical speed before adding jump
		this->actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;

		SETVECTOR(this->actorInstance->actorBehaviour->physics.velocity, ball.speed*bmSin(ball.getYang())*0.5f, koikSpeed, ball.speed*bmCos(ball.getYang())*0.5f, 1.0f);
		this->actorInstance->actorBehaviour->physics.speed = bmVectorLen(this->actorInstance->actorBehaviour->physics.velocity);

		bmVectorCopy(startVel, actorInstance->actorBehaviour->physics.velocity);

		//PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.2f,JUMP);
		this->actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
		this->actorInstance->actorAnimation->useMotionBone = FALSE;
		//PlaySample("taz start jump",255,actorInstance->actorInstance.position);
		//this->actorInstance->actorStatus->currentState = STATE_RECOVER;// PP: was _JUMP
	}

	// PP: switch to recovery state

	RequestStateChange(this->actorInstance,STATE_RECOVER);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														ZBBALL methods

									This is the 'ball' (Elephant) used in the Zoo Boss game

															@zbball
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: Messages for the ball
#define ZBBALLMESSAGE_WALLTURN				"wallturn"				// PP: ball should now start turning as the result of a collision with a player's wall
#define ZBBALLMESSAGE_FOOTSTEP_FRONTLEFT	"frontleft"				// PP: elephant's front left foot hits the ground
#define ZBBALLMESSAGE_FOOTSTEP_FRONTRIGHT	"frontright"			// PP: elephant's front right foot hits the ground
#define ZBBALLMESSAGE_FOOTSTEP_REARLEFT		"rearleft"				// PP: elephant's rear left foot hits the ground
#define ZBBALLMESSAGE_FOOTSTEP_REARRIGHT	"rearright"				// PP: elephant's rear right foot hits the ground
#define ZBBALLMESSAGE_FOOTSTEP_ALL			"all"					// PP: all elephant's feet hit the ground
#define ZBBALLMESSAGE_CLOSEEYES				"eyeclose"				// PP: close elephant's eyes
#define ZBBALLMESSAGE_OPENEYES				"eyeopen"				// PP: close elephant's eyes

#define ZBBALL_NO_CURRENT_ANIM		(!CheckIfPlayingAnimation(this->actorInstance, NULL))

#define ZBBALL_HEARING_ANGLE_RANGE	(0.9f)		// PP: in either direction

#define ZBBALL_HEARING_RADIUS		METERTOUNIT(5)

#define ZBBALL_MAX_BLINK_INTERVAL	1.0f

// PP: duration of an elephant blink (seconds)
#define ZBBALL_BLINK_DURATION		0.1f


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::init
	Purpose 	: initialise the ball
	Parameters 	: position, angle, court, (opt/ZBBALLTYPE_ELEPHANT) type for ball
	Returns 	: 
	Info 		: 
*/

// PP: names of foot bones corresponding to each of the elephant's steps, in order
char* ZBBALL_stepBoneNames[4]=
{
	"lrearleftfoot", 
	"lfrontleftfoot",
	"lrearrightfoot",
	"lfrontrightfoot"
};

#define ZBBALL_FOOTPRINT_TEXTURE				"extras\\elephant footprint.bmp"

void ZBBALL::init(TBVector pos, const float ang, const RECTANGLE& courtIn, const ZBballType type)
{
	// PP: NOTE: I'm going to completely ignore the type parameter for just now - it's creature feep

	// PP: initialise static footprint texture
	ZBBALL::footprintTexture = bkLoadTexture(NULL, ZBBALL_FOOTPRINT_TEXTURE, 0);

	// PP: set radii for the ball
	this->innerCollRadius=ZBBALL_INNERCOLLRADIUS_ELEPHANT;
	this->outerCollRadius=ZBBALL_OUTERCOLLRADIUS_ELEPHANT;
	this->tuskCollRadius=ZBBALL_TUSKCOLLRADIUS_ELEPHANT;

	// PP: set the court for the ball
	this->court=courtIn;

	// PP: set up the 'contact court'
	this->contactCourt.left=this->court.left+this->innerCollRadius;
	this->contactCourt.right=this->court.right-this->innerCollRadius;
	this->inContactCourt=true;// PP: why not

	float wallCollideRadius=this->outerCollRadius*1.3f;			// PP: REVIEW THIS	
	this->contactCourt.bottom=this->court.bottom+wallCollideRadius;
	this->contactCourt.top=this->court.top-wallCollideRadius;

	// PP: create the actor instance for the ball
	this->actorInstance=ZooBoss_game.createActorInstance("elephant.obe", pos, ang, BGACT_CARTOON|BGACT_ANIMATION);
//	RemoveActorFromCollisionCache(&collisionCache, this->actorInstance);

	// PP: find your feet, so to speak...

	for(int f=0; f<4; f++)
	{
		this->steps[f].node = baFindNode(this->actorInstance->actorInstance.rootNodeInstance, ZBBALL_stepBoneNames[f]);
	}

	// PP: find heed
	this->headBone=baFindNode(this->actorInstance->actorInstance.rootNodeInstance, "trunk1");

	// PP: create a shadow for the ball
	if(VFX_effectActive(VFX_SHADOWMAP))
	{
		CreateActorInstanceShadow(this->actorInstance,METERTOUNIT(4.5f),METERTOUNIT(10), SHADOWMAP_RADIUS_ELEPHANT);
	}
	else
	{
		CreateActorInstanceShadow(this->actorInstance,METERTOUNIT(4.5f),METERTOUNIT(10), SHADOWMAP_RADIUS_ELEPHANT, SHADOWTYPE_ZOOBOSS);
	}

	TBVector		tempVector;
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);

	// PP: set the ball's direction
	// PP: it makes sense for the ball to run towards gossamer first, so's the player gets a chance to see what's going on
	this->setYang(ang);
	this->destYang=this->getYang();

	// PP: initialise animation
	this->runPhase=0.0f;
	this->speedFraction=ZBBALL::initialSpeedFractions[CURRENT_DIFFICULTY];

	this->flags=ZBBALL_INITIAL_FLAGS;

	this->oldState=ZBBALL_INITIAL_STATE;

	this->TBwallContactLatch=false;
	this->RLwallContactLatch=false;

	for(int i=0;i<BOSS_NUM_PLAYERS;i++)
	{
		this->batInnerCollLatch[i]=false;
		this->batOuterCollLatch[i]=false;
		this->batTuskCollLatch[i]=false;
		this->batHearLatch[i]=false;
		this->batScareLatch[i]=false;
	}

	// PP: initialise anims
	this->anims.walk=baFindAnimSegment(&(this->actorInstance->actorInstance), "walk1");
	this->anims.run=baFindAnimSegment(&(this->actorInstance->actorInstance), "run");

	this->anims.jump=baFindAnimSegment(&(this->actorInstance->actorInstance), "scared2");
	this->anims.skidJump=baFindAnimSegment(&(this->actorInstance->actorInstance), "scared1");
	this->anims.quake=baFindAnimSegment(&(this->actorInstance->actorInstance), "earthquakeshake");

	this->anims.wallKoik=baFindAnimSegment(&(this->actorInstance->actorInstance), "wallbounce1");// PP: temp test

	// PP: initialise hearing stuff
	// PP: TEMP no hearing offset TODO:  position at trunk1 (head)?
	this->hearingOffset[0]=0.0f;
	this->hearingOffset[1]=0.0f;

	this->setState(ZBBALL_INITIAL_STATE);

	// PP: yeux

	// PP: get hold of elephant's eyes (for game rather than cutscene)
	this->eyeTexture=bkLoadTexture(NULL, ZBBALL_GAME_EYE_TEXTURE, 0);

	this->interBlinkTimer=bmRandf()*ZBBALL_MAX_BLINK_INTERVAL;
	this->blinkTimer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::initPreCutscene
	Purpose 	: initialisation before the intro cutscene
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::initPreCutscene(void)
{
	// PP: not found cutscene bone yet
	this->cutsceneNode=NULL;

	// PP: set up the watchtower collision box...

#define BOXWIDTH	METERTOUNIT(2)
#define BOXHEIGHT	METERTOUNIT(12)
#define BOX_X		0
#define	BOX_Z		(-((ZB_COURT_WIDTH*0.5f)+BOXWIDTH+METERTOUNIT(1)))
#define BOX_Y		ZB_COURT_HEIGHT

	SETVECTOR(this->towerBox.minBox, BOX_X-BOXWIDTH, BOX_Y, BOX_Z-BOXWIDTH, 1.0f);
	SETVECTOR(this->towerBox.maxBox, BOX_X+BOXWIDTH, BOX_Y+BOXHEIGHT, BOX_Z+BOXWIDTH, 1.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::setState
	Purpose 	: set the state of the ball
	Parameters 	: (opt/ZBBALLTYPE_ELEPHANT) type for ball
	Returns 	: 
	Info 		: 
*/

#define ZBBALL_INTRODROP_HEIGHT		(ZB_COURT_HEIGHT+METERTOUNIT(80))
#define ZBBALL_GOALDROP_HEIGHT		(ZB_COURT_HEIGHT+METERTOUNIT(10))
#define ZBBALL_INTRODROP_SPEED		METERTOUNIT(10)
#define ZBBALL_GOALDROP_SPEED		METERTOUNIT(18)
#define ZBBALL_GRAVITY				METERTOUNIT(-9.81f)						// PP: *clap clap clap* oh how imaginative.

#define ZBBALL_MIN_MOVE_DIST			METERTOUNIT(3)
#define ZBBALL_MAX_MOVE_DIST			METERTOUNIT(12)

void ZBBALL::setState(const ZBballState newState)
{
	this->flags &= ~ZBBALLFLAG_NEWDIRECTION;
	this->flags &= ~ZBBALLFLAG_NEWSTATE;

	this->oldState=this->state;

	switch(newState)
	{
	case ZBBALLSTATE_INTRODROP1:
		{
			ZooBoss_game.setCamera(ZBCAM_INTRODROP);

			PlayAnimationByName(this->actorInstance,"fly",1.0f, 1, 0, 0.1f, NONE);

			// PP: position the elephant waay up on high
			SETVECTOR(this->pos(), 0.0f, ZBBALL_INTRODROP_HEIGHT, 0.0f, 1.0f);

			// PP: it's already falling at a fair rate
			this->speed= -METERTOUNIT(10);

			// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
			this->flags|=ZBBALLFLAG_NEWDIRECTION;

			// PP: play divebomb sound
			PlayTrackingSample("elephant_fall.wav", 255, this->pos(), 0);
		}
		break;

	case ZBBALLSTATE_GOALDROP:
		{
#ifdef ZB_SPINOFF
			if(videoMode.numDraws == 1)// PP: no spinoff in stereoscopic mode; don't have time to get it working
			{
				// PP: prepare a spinoff effect to start on next frame
				VFX_spinoffPrepare();
			}

#endif// PP: def ZB_SPINOFF

			this->actorInstance->flags &= ~ACTORFLAG_DONTDRAW;

			PlayAnimationByName(this->actorInstance,"fly",1.0f, 1, 0, 0.1f, NONE);

			// PP: it's already falling at a fair rate
			this->speed= -METERTOUNIT(10);

			// PP: trigger gosamer's AI
			this->flags|=ZBBALLFLAG_NEWSTATE;

			// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
			this->flags|=ZBBALLFLAG_NEWDIRECTION;

			// PP: people seem to hate the fact that the elephant's speed doesn't reset to minimum after each goal,
			// PP: so now it does
			this->speedFraction=ZBBALL::initialSpeedFractions[CURRENT_DIFFICULTY];
		}
		break;

	case ZBBALLSTATE_GOAL:
		{
			this->flags &= ~ZBBALLFLAG_TURNING;

			this->actorInstance->flags |= ACTORFLAG_DONTDRAW;

			// PP: position the elephant waay up on high
			SETVECTOR(this->pos(), 0.0f, ZBBALL_GOALDROP_HEIGHT, 0.0f, 1.0f);

			// PP: either face towards Taz or towards Gossamer
			if(bmRand()&1)
			{
				this->destYang=HALFPI;
			}
			else
			{
				this->destYang=PI+HALFPI;
			}

			this->setYang(this->destYang);

			// PP: talk to the bats
			for(int p=0; p< BOSS_NUM_PLAYERS; p++)
			{
				if(this->koikWall == ZooBoss_game.bats[p].wall)
				{					
					//ZooBoss_game.bats[p].setState(ZBBATSTATE_LOSING);
				}
				else
				{
					ZooBoss_game.winner=p;
					ZooBoss_game.loser=ZooBoss_game.winner^1;
					ZooBoss_game.bats[ZooBoss_game.winner].setState(ZBBATSTATE_WINNING);
				}
			}

			if(ZooBoss_game.twoPlayer)
			{
				if(ZooBoss_game.bats[ZooBoss_game.winner].getScore() == ZB_MAX_SCORE_2PLAYER)
				{
					ZooBoss_game.setState(BGSTATE_OUTRO);
				}
				else
				{
					ZooBoss_game.setCamera(ZBCAM_GOALWINNER);
				}
			}
			else
			{
				ZooBoss_game.setCamera(ZBCAM_GOALWINNER);
			}
		}
		break;

	case ZBBALLSTATE_INTROID:
		{
			// PP: stop all sound
			StopAllChannels();
			ZooBoss_messageBook->open("ELEPHANTID", true);

			SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, int32(ZooBoss_introIDpauseUpdateFunc), int32(this));

			this->timer=0.0f;// PP: this will tick up to ZBBALL_DURATION_INTROID
		}
		break;

	case ZBBALLSTATE_INTRODROP2:
		{
			ZooBoss_messageBook->close();
			SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

			// PP: play piano-ground-imapact sound
			PlayTrackingSample("elephant_land.wav", 255, this->pos(), 0);
		}
		break;

	case ZBBALLSTATE_INTROLAND:
		{
			this->pos()[Y]=ZB_COURT_HEIGHT;


			// PP: TEMP TEST
			// PP: flush the node queue to stop nasty things happening
			baFlushNodeQueue(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance, TRUE);


			PlayAnimationByName(this->actorInstance,"landingbounce",1.0f, 0, 0, 0.1f, NONE);
		}
		break;

	case ZBBALLSTATE_INTROSHAKE:
		{
			if(ZooBoss_game.state == BGSTATE_INTRO)
			{
				ZooBoss_messageBook->open("PREPAREZVOUS");
			}

			// PP: castanets sound
			this->playSound("eleshakehead.wav", AIUSOUND_POSITION);

			PlayAnimationByName(this->actorInstance,"idle3",1.0f, 0, 0, 0.1f, NONE);
		}
		break;

	case ZBBALLSTATE_INTROHOOT:
		{
			if(ZooBoss_game.state == BGSTATE_INTRO)
			{
				ZooBoss_game.setCamera(ZBCAM_INTROHOOT);
			}

			// PP: elephant trumpet sound
			this->playSound("elephant.wav", AIUSOUND_POSITION);

			PlayAnimationByName(this->actorInstance,"idle2",1.0f, 0, 0, 0.1f, NONE);
		}
		break;

	case ZBBALLSTATE_RUNNING:
		{
			if(ZooBoss_game.state == BGSTATE_INTRO)
			{
				if(this->oldState < ZBBALLSTATE_RUNNING)
				{
					ZooBoss_game.setCamera(ZBCAM_PLAY);

					ZooBoss_messageBook->gotoPage("GO");
					ZooBoss_messageBook->close(ZB_DURATION_STANDARDMESSAGE);
				}
			}

			// PP: stop using the motion bone now - we're going to start doin mo blended run thang yall
			this->actorInstance->actorAnimation->useMotionBone=FALSE;

			// PP: ADDED TEST - wasn't here for some reason
			// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
			//this->flags|=ZBBALLFLAG_NEWDIRECTION;

			this->TBwallContactLatch=false;
			this->RLwallContactLatch=false;
			
			// PP: TEST - force final angle to be dest angle
			//bkPrintf("WALLKOIK angle error %5.2f\n", this->destYang-this->getYang());
			this->setYang(this->destYang);

			// PP: ~~~

			// PP: set new speed - this is needed by the bat ai
			this->speed=ZBBALL_MIN_MOVE_DIST+(this->speedFraction*(ZBBALL_MAX_MOVE_DIST-ZBBALL_MIN_MOVE_DIST));
			
			// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
			this->flags|=ZBBALLFLAG_NEWDIRECTION;
		}
		break;

	case ZBBALLSTATE_WALLBOUNCING:
		{
			/*
			// PP: reset timer
			this->timer=0.0f;

			// PP: use the motion bone for this anim
			this->actorInstance->actorAnimation->useMotionBone=TRUE;

			PlayAnimationBySegmentPointer(this->actorInstance,this->anims.wallKoik,1.0f, 0, 0, 0.1f, NONE);
			*/

			// PP: head to centre of court - jees, this saves so much hassle
			this->destYang=Aabs(bmATan2(this->court.midY()-this->pos()[Z], this->court.midX()-this->pos()[X])-HALFPI);

			this->srcYang=this->getYang();
			// PP: start koik
			this->state=ZBBALLSTATE_WALLBOUNCING;
			// PP: use the motion bone for this anim
			this->actorInstance->actorAnimation->useMotionBone=TRUE;
			PlayAnimationBySegmentPointer(this->actorInstance,this->anims.wallKoik,1.0f, 0, 0, 0.1f, NONE);
			
			this->timer=0.0f;
		}
		break;

	case ZBBALLSTATE_QUAKING:
		{
			this->timer=0.0f;
		}
		break;


	case ZBBALLSTATE_SKIDDING:
		{
			this->timer=0.0f;
		}
		break;

	case ZBBALLSTATE_JUMPING:
		{
			this->timer=0.0f;
		}
		break;
	
	case ZBBALLSTATE_AWAITINGSTOMP:
		{
			if(this->state == ZBBALLSTATE_AWAITINGSTOMP)
			{
				bkPrintf("schtopp!\n");
			}

			// PP: halt the elephant - wait for ground impact
			FlushAnimationQueue(this->actorInstance);
		}
		break;
	}

	bkPrintf("ZBBALL::setState %d to %d\n", this->state, newState);

	this->state=newState;

	this->flags|=ZBBALLFLAG_NEWSTATE;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::update
	Purpose 	: update the ball
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

/*
	ELEPHANT STEPS ARE:

  0			rear left
  0.25		front left
  0.5		rear right
  0.75		front right
*/

#define ZBBALL_DURATION_INTROID					2.25f
#define ZBBALL_DURATION_SKID					1.33f
#define ZBBALL_SPEEDUP_FRACTION					0.04f
#define ZBBALL_SPEED_SKIDHALT					0.8// PP: 0.5f				// PP: speed fraction above which elephant will skid to a halt rather than just jump

void ZBBALL::update(void)
{

#ifdef ZOOBOSS_DEBUG// PP: if debug options are enabled
/*
	if(controller1.l1Channel->value != 0)
	{
		this->speedFraction=CONSTRAIN(this->speedFraction+0.07f, 0.0f, 1.0f);
	}

	if(controller1.l2Channel->value != 0)
	{
		this->speedFraction=CONSTRAIN(this->speedFraction-0.07f, 0.0f, 1.0f);
	}

	if(controller1.r3DebounceChannel->value != 0)
	{
		this->destYang=Aabs(this->getYang()+(HALFPI/2.0f));// PP: temp
		this->flags|=ZBBALLFLAG_TURNING;
	}
*/
#endif// PP: def ZOOBOSS_DEBUG// PP: if debug options are enabled

	switch(this->state)
	{
	case ZBBALLSTATE_RUNNING:
		{
			// PP: create dust trail...
			// PP: believe it or not, this continuous trail works much better than puffs on each footstep

			// PP: yadda
#define ZBBALL_RUN_DUST_FREQUENCY		runDustFreq
#define ZBBALL_RUN_DUST_POS_DEVIATION	(bmRandfSigned()*300.0f)

#ifdef ZB_PONGDUST

			PongDust_create(this->pos()+VEC(ZBBALL_RUN_DUST_POS_DEVIATION,0,ZBBALL_RUN_DUST_POS_DEVIATION), 1, ZVEC, ZBBALL_RUN_DUST_FREQUENCY*this->speedFraction);

#endif// PP: pongdust

			// PP: udpate head pos
			baGetNodesWorldPosition(&(this->actorInstance->actorInstance), this->headBone, this->headPos);

			// PP: update the run phase
			this->runPhase=this->runPhase+((1/(float)(ANIMFRAMES(this->anims.walk)/*/BTICKSPERFRAME*/)*60/fps)*(1.0f+this->speedFraction));

			// PP: footsteps...

			int		f;

			if(this->runPhase > 1.0f)
			{
				// PP: reset steps

				for(f=0;f<4;f++)
				{
					this->steps[f].done=false;
				}

				this->runPhase=(float)bmFMod(this->runPhase, 1.0f);
			}

			for(f=0; f<4; f++)
			{
				if(!this->steps[f].done)
				{
					if(this->runPhase > (0.25f*f))
					{
						this->footstep(this->actorInstance, f);
						
						this->steps[f].done=true;
					}
				}
			}

			// PP: pose the ball according to its current point in its walk/run
			unsigned int walkFrame=bmFloatToInt(this->anims.walk->startFrame+(this->runPhase*ANIMFRAMES(this->anims.walk)*BTICKSPERFRAME));
			unsigned int runFrame=bmFloatToInt(this->anims.run->startFrame+(this->runPhase*ANIMFRAMES(this->anims.run)*BTICKSPERFRAME));

			baSetNodeBlend(&(this->actorInstance->actorInstance), this->actorInstance->actorInstance.rootNodeInstance, walkFrame, runFrame, this->speedFraction, 1);

#ifndef ZB_STATIC_BALL
			this->applyNormalMovement(this->getYang());
#endif// PP: ndef ZB_STATIC_BALL

			// PP: test for, and respond to, collisions with the court walls
			this->testWallCollisions();

			// PP: if the ball has just entered its contact court, flag it as having done so...

			bool tempBool=this->contactCourt.contains(this->pos()[X], this->pos()[Z]);

			if(!this->inContactCourt)
			{
				if(tempBool)
				{
					this->flags |= ZBBALLFLAG_NEWLYENTEREDCONTACTCOURT;
				}
			}

			this->inContactCourt=tempBool;
		}
		break;


	case ZBBALLSTATE_WALLBOUNCING:
		{
			if(ZBBALL_NO_CURRENT_ANIM)
			//if(controller1.r1Channel->value != 0)
			{
				this->setState(ZBBALLSTATE_RUNNING);
			}
			else
			{
				float		turnFraction;		// PP: fraction of turn to apply

				//bmVectorAdd(this->pos(), this->pos(), this->actorInstance->actorAnimation->motion);

				// PP: ~~~~~~~~~~~~~~~~~~#


#define ZBBALL_DURATION_WALLKOIKFIRSTTURN			0.22f

				if(this->timer < ZBBALL_DURATION_WALLKOIKFIRSTTURN)
				{
					// PP: The elephant is compressing against the wall - turn it to face backwards along its rebound path

					turnFraction=this->timer/(float)ZBBALL_DURATION_WALLKOIKFIRSTTURN;
					this->setYang(this->srcYang+(turnFraction*FindShortestAngleSigned(this->destYang, this->srcYang)));

				//	bkPrintf("timer: %5.2f, turnFraction: %5.2f, srcYang: %5.2f, destYang: %5.2f, yAng: %5.2f\n", this->timer, turnFraction, this->srcYang, this->destYang, this->getYang());

					// PP: APPLY MOTION, rotated with actor instance

					bmVectorAdd(this->pos(), this->pos(), this->actorInstance->actorAnimation->motion);

					// PP: take this frame's time from the remaining turn time
					this->timer+=(1/fps);

					if(this->timer > ZBBALL_DURATION_WALLKOIKFIRSTTURN)
					{
						// PP: set the next destination angle
						this->destYang=Aabs(this->getYang()+PI);

						this->srcYang=this->getYang();
					}
				}
				else
				{
					/* - use the motion bone...

					TBVector		finalMotion;

					//SETVECTOR(src, 0,0,10,0);

					bmVectorRotateY(finalMotion, this->actorInstance->actorAnimation->motion, Aabs(this->getYang()-this->srcYang));

					bmVectorAdd(this->pos(), this->pos(), finalMotion);

					/*/ // - move it manually...

					this->applyNormalMovement(this->destYang);

					//*/

					//if(this->flags & ZBBALLFLAG_TURNING)// PP: as set by the 'wallturn' message
					{
						turnFraction=(this->timer-ZBBALL_DURATION_WALLKOIKFIRSTTURN)/(ANIMTIME(this->anims.wallKoik)-ZBBALL_DURATION_WALLKOIKFIRSTTURN);
						this->setYang((this->srcYang+(turnFraction*FindRandomlySignedAngle(this->destYang, this->srcYang))));
					}

					this->timer+=(1/fps);

					// PP: test for, and respond to, collisions with the court walls
				//	this->testWallCollisions();
				}
			}
		}
		break;

	case ZBBALLSTATE_QUAKING:
		{
			if(this->timer < ANIMTIME(this->anims.quake))
			{
				float		turnFraction;		// PP: fraction of turn to apply

				turnFraction=(this->timer-ZBBALL_DURATION_SKID)/(ANIMTIME(this->anims.skidJump)-ZBBALL_DURATION_SKID);
				this->setYang(this->srcYang+(turnFraction*FindShortestAngleSigned(this->srcYang, this->destYang)));

				// PP: take this frame's time from the remaining turn time
				this->timer+=(1/fps);
			}
			else
			{
				// PP: end the turn, back to running state
				this->setState(ZBBALLSTATE_RUNNING);

				// PP: speed up the elephant's run a bit now
				this->speedFraction+=ZBBALL_SPEEDUP_FRACTION;
				// PP: REMOUT: TEST			this->speedFraction=CONSTRAIN(this->speedFraction, 0.0f, 1.0f);

				// PP: set new speed - this is needed by the bat ai
				this->speed=ZBBALL_MIN_MOVE_DIST+(this->speedFraction*(ZBBALL_MAX_MOVE_DIST-ZBBALL_MIN_MOVE_DIST));

				// PP: set the new state flag
				this->flags|=ZBBALLFLAG_NEWSTATE;

				// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
				//this->flags|=ZBBALLFLAG_NEWDIRECTION;
			}
		}
		break;


	case ZBBALLSTATE_JUMPING:
		{
			// PP: check for end of anim
			if(ZBBALL_NO_CURRENT_ANIM)
			{
				// PP: end the turn, back to running state
				this->setState(ZBBALLSTATE_RUNNING);

				// PP: speed up the elephant's run a bit now
				this->speedFraction+=ZBBALL_SPEEDUP_FRACTION;
				// PP: REMOUT: TEST			this->speedFraction=CONSTRAIN(this->speedFraction, 0.0f, 1.0f);

				// PP: set new speed - this is needed by the bat ai
				this->speed=ZBBALL_MIN_MOVE_DIST+(this->speedFraction*(ZBBALL_MAX_MOVE_DIST-ZBBALL_MIN_MOVE_DIST));
				
				// PP: set the new state flag
				this->flags|=ZBBALLFLAG_NEWSTATE;

				// PP: set the new direction flag to force the ai bat(s) to recalculate the ball's path
				//this->flags|=ZBBALLFLAG_NEWDIRECTION;
			}
			else
			{
				// PP: elephant is jumping - turn it to its destination angle
				
				float		turnFraction;		// PP: fraction of turn to apply

				turnFraction=this->timer/this->duration;
				this->setYang(this->srcYang+(turnFraction*FindShortestAngleSigned(this->destYang, this->srcYang)));

				// PP: take this frame's time from the remaining turn time
				this->timer+=(1/fps);
			}
		}
		break;

	case ZBBALLSTATE_SKIDDING:
		{
			if(this->timer < ZBBALL_DURATION_SKID)
			{
  				bmVectorAdd(this->pos(), this->pos(), this->actorInstance->actorAnimation->motion);

#ifdef ZB_PONGDUST

				// PP: frequency of dust particle emission during skid
#define ZBBALL_SKIDSTOP_DUST_FREQUENCY		5

				// PP: KICK UP SOME DUST

				VEC		dustVel;

				// PP: dust velocity is elephant's velocity...
				dustVel=this->actorInstance->actorAnimation->motion;

				// PP: ...exaggerated a wee bit
				dustVel*=1.2f;
				
				PongDust_create(this->actorInstance, "lfrontrightfoot", 1, dustVel, ZBBALL_SKIDSTOP_DUST_FREQUENCY);
				PongDust_create(this->actorInstance, "lfrontleftfoot", 1, dustVel, ZBBALL_SKIDSTOP_DUST_FREQUENCY);
				PongDust_create(this->actorInstance, "lrearrightfoot", 1, ZVEC, ZBBALL_SKIDSTOP_DUST_FREQUENCY);
				PongDust_create(this->actorInstance, "lrearleftfoot", 1, ZVEC, ZBBALL_SKIDSTOP_DUST_FREQUENCY);

#endif// PP: pongdust

				// PP: take this frame's time from the remaining skid time
				this->timer+=fTime;
			}
			else// PP: finished skid
			{
	/*			if(this->scareType == SCARE_STOMP)
				{
// PP: tst					this->setState(ZBBALLSTATE_AWAITINGSTOMP);
				}
				else*/
				{
					this->duration=ANIMTIME(this->anims.skidJump)-this->timer;// PP: remaining time in the combined skid-jump anim will be duration of jump state

					this->setState(ZBBALLSTATE_JUMPING);
				}
			}
		}
		break;

	case ZBBALLSTATE_INTRODROP1:
		{
			this->pos()[Y]+=(this->speed*fTime);

			if(this->pos()[Y] < ZBBALL_INTROID_HEIGHT)
			{
				this->setState(ZBBALLSTATE_INTROID);
			}

			this->speed+=(ZBBALL_GRAVITY*fTime);
		}
		break;


	case ZBBALLSTATE_GOALDROP:
		{

#ifdef ZB_SPINOFF
			if((videoMode.numDraws>1) || VFX_spinoffNewlyPrepared)// PP: no spinoff in stereoscopic mode; don't have time to get it working
#endif// PP: def ZB_SPINOFF	
			{
				if(VFX_spinoffNewlyPrepared)
				{
					VFX_spinoffNewlyPrepared=false;
				}

				// PP: spinoff frame has been written to the VFX_renderTarget, so we're ready now to change camera shot
				ZooBoss_game.setCamera(ZBCAM_PLAY);

#ifdef ZB_SPINOFF
				if(videoMode.numDraws == 1)// PP: no spinoff in stereoscopic mode; don't have time to get it working
				{
					// PP: VFX_spinoffStart has to be called from outside VideoFX because there might be a time when
					// PP:		you want to spin off a frame that was prepared some time ago
					VFX_spinoffStart();
				}
#endif// PP: def ZB_SPINOFF	

				// PP: VFX_spinoffStarted is now true
			}

			this->pos()[Y]+=(this->speed*fTime);

			if(this->pos()[Y] < ZB_COURT_HEIGHT)
			{
				this->setState(ZBBALLSTATE_INTROLAND);
			}

			this->speed+=(ZBBALL_GRAVITY*fTime);
		}
		break;

	case ZBBALLSTATE_GOAL:
		{
			
		}
		break;

	case ZBBALLSTATE_INTROID:
		{
			// PP: TODO: that idea of having the elephant blink would be nice

			this->timer += fTime;
			if(this->timer > ZBBALL_DURATION_INTROID)
			{
				this->setState(ZBBALLSTATE_INTRODROP2);
			}
		}
		break;

	case ZBBALLSTATE_INTRODROP2:
		{
			this->pos()[Y]+=(this->speed*fTime);

			if(this->pos()[Y] < ZB_COURT_HEIGHT)
			{
				this->setState(ZBBALLSTATE_INTROLAND);
			}

			this->speed+=(ZBBALL_GRAVITY*fTime);
		}
		break;

	case ZBBALLSTATE_INTROLAND:
		{
			if(ZBBALL_NO_CURRENT_ANIM)
			{
				if(ZooBoss_game.state == BGSTATE_INTRO)
				{
					this->setState(ZBBALLSTATE_INTROSHAKE);
				}
				else
				{
					this->setState(ZBBALLSTATE_INTROHOOT);
				}
			}
		}
		break;

	case ZBBALLSTATE_INTROSHAKE:
		{
			if(ZBBALL_NO_CURRENT_ANIM)
			{
				this->setState(ZBBALLSTATE_INTROHOOT);
			}
		}
		break;

	case ZBBALLSTATE_INTROHOOT:
		{
			if(ZBBALL_NO_CURRENT_ANIM)
			{
				this->setState(ZBBALLSTATE_RUNNING);
			}
		}
		break;
	}// PP: end switch

	// PP: whatever state the ball's in, make sure it's in the court
	this->court.contain(this->pos(), this->innerCollRadius);

	// PP: ONLY USE THIS FOR NORMAL TURNING IN RESPONSE TO A WALL BRUSH!!!
	// PP: continue turning towards destination angle
	if(this->flags & ZBBALLFLAG_TURNING)
	{
		float	shortestAngle;

		shortestAngle=FindShortestAngleSigned(this->destYang, this->getYang());

		// PP: enforce a what-do-you-call-it - epsilon?
		if(bmFAbs(shortestAngle) < 0.001f)
		{
			shortestAngle=0.0f;
			this->flags&= ~ZBBALLFLAG_TURNING;		// PP: finished the turn
			this->flags|=ZBBALLFLAG_NEWDIRECTION;	
		}
		else
		{
			float	turnStep=this->getTurnSpeed()/fps;

			if(shortestAngle > 0.0f)
			{
				this->setYang(Aabs(this->getYang()+MIN(turnStep, shortestAngle)));
			}
			else if(shortestAngle < 0.0f)
			{
				this->setYang(Aabs(this->getYang()-MIN(turnStep, -shortestAngle)));
			}
		}
	}

	// PP: ===============
	// PP: UPDATE SHADE...
	// PP: ===============

	TBCollisionInfo		info;

	bmVectorCopy(this->towerRay.start, this->pos());
	this->towerRay.start[Y]+=ZBBALL_SUN_BLOCKAGE_HEIGHT;

	bmVectorCopy(this->towerRay.unitVector, VEC(0,0,-ZB_SUN_BLOCKAGE_RAY_LENGTH).rotate(ZooBoss_shadowAng));

	if(bmCollideRayAABB(&this->towerRay, &this->towerBox, &info))
	{
		this->actorInstance->flags |= ACTORFLAG_INSHADE;
	}
	else
	{
		this->actorInstance->flags &= ~ACTORFLAG_INSHADE;
	}

	// PP: ===============
	// PP: UPDATE EYES...
	// PP: ===============

	// PP: random blinking...
	if(this->interBlinkTimer > 0.0f)
	{
		if((this->interBlinkTimer -= fTime) <= 0.0f)
		{
			this->controlEyes(ZBBALLEYES_BLINK);
			
			this->interBlinkTimer=bmRandf()*ZBBALL_MAX_BLINK_INTERVAL;
		}
	}

	if(this->blinkTimer > 0.0f)
	{
		if((this->blinkTimer -= fTime) <= 0.0f)
		{
			// PP: blink finished; open eyes again
			this->controlEyes(ZBBALLEYES_OPEN);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::draw
	Purpose 	: draw special stuff the ball
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::draw(void) const
{
	if(ZooBoss_debug)
	{
		utilDrawCircle(this->pos(), this->outerCollRadius, COLOUR(0,0,255));
		utilDrawCircle(this->pos(), this->innerCollRadius, COLOUR(255,0,0));

		utilDrawCircleSector(this->pos(), this->getYang(), ZBBALL_HEARING_RADIUS, ZBBALL_HEARING_ANGLE_RANGE, COLOUR(0,255,0));
		utilDrawCircleSector(this->pos(), this->getYang(), tuskCollRadius, ZBBALL_TUSK_ANGLE_RANGE, COLOUR(255,0,0));
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::shutdown
	Purpose 	: shutdown the ball
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::shutdown(void)
{

}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::respondToMessage
	Purpose 	: respond to a message
	Parameters 	: ptr to ACTORINSTANCE generating the message, message string to which to respond
	Returns 	: true if the message was recognised and handled, else false
	Info 		: 
*/
bool ZBBALL::respondToMessage(const ACTORINSTANCE* const messageActorInstance, const char* const message)
{
	if(stricmp(message, ZBBALLMESSAGE_FOOTSTEP_FRONTLEFT) == 0)
	{
		this->footstep(messageActorInstance, ZBBALLFOOT_FRONTLEFT);
	}
	else if(stricmp(message, ZBBALLMESSAGE_FOOTSTEP_FRONTRIGHT) == 0)
	{
		this->footstep(messageActorInstance, ZBBALLFOOT_FRONTRIGHT);
	}
	else if(stricmp(message, ZBBALLMESSAGE_FOOTSTEP_REARLEFT) == 0)
	{
		this->footstep(messageActorInstance, ZBBALLFOOT_REARLEFT);
	}
	else if(stricmp(message, ZBBALLMESSAGE_FOOTSTEP_REARRIGHT) == 0)
	{
		this->footstep(messageActorInstance, ZBBALLFOOT_REARLEFT);
	}
	else if(stricmp(message, ZBBALLMESSAGE_FOOTSTEP_ALL) == 0)
	{
		this->footstep(messageActorInstance, ZBBALLFOOT_ALL);
	}
	else if(stricmp(message, ZBBALLMESSAGE_CLOSEEYES) == 0)
	{
		this->controlEyes(ZBBALLEYES_CLOSE);
	}
	else if(stricmp(message, ZBBALLMESSAGE_OPENEYES) == 0)
	{
		this->controlEyes(ZBBALLEYES_OPEN);
	}
	else
	{
		return false;
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::quake
	Purpose 	: quake in response to Gossamer's stomp hitting the ground
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::quake(void)
{
	// PP: we don't want the ball to be quaked while it's up in the air
	//if(this->state != ZBBALLSTATE_GOAL)
	{
		// PP: play the quake anim
		this->playAnim(this->anims.quake);

		this->setState(ZBBALLSTATE_QUAKING);
	}
}


#define ZBBALL_SKID_TIME				1.33f
#define ZBBALL_DEFLECTION_MAX_ANGDIFF	RAD(50)// PP: RAD(60)				// PP: maximum angle difference from the deflecting bat
/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::beDeflected
	Purpose 	: be deflected by a bat
	Parameters 	: bat by which to be deflected
	Returns 	: 
	Info 		: 
*/
void ZBBALL::beDeflected(const struct TAG_ZBBAT& bat)
{
	// PP: keep a record of the type of scare being done to the elephant
	this->scareType=bat.scareType;

	if(bat.scareType == SCARE_SUPERSPIN)
	{
		// PP: TODO: award a cheat?
		PlaySample("applause.wav");
	}

	// PP: TEST: clear footstep latches
	for(int f=0; f<4; f++)
	{
		this->steps[f].done=false;
	}

	// flush the animation queue, saving the current state in the process
	baFlushNodeQueue(&(this->actorInstance->actorInstance), this->actorInstance->actorInstance.rootNodeInstance, TRUE);

	if(this->speedFraction > ZBBALL_SPEED_SKIDHALT)// PP: GOING FAST
	{
		// PP: play that luvverly skid sound *SCREITCH!*
		this->playSound("eleskid.wav", AIUSOUND_TRACK, 128);

		// PP: ...AND play the frightened sound why the fook not
		this->playSound("elescaredjump.wav", AIUSOUND_TRACK);

		// PP: TEST: USE MOTION BONE
		this->actorInstance->actorAnimation->useMotionBone=TRUE;

		// PP: SKID TO A HALT, THEN JUMP WHILE TURNING TO FACE THE OTHER WAY
		baQueueNodeAnimBlendState(&(this->actorInstance->actorInstance), this->actorInstance->actorInstance.rootNodeInstance, this->anims.skidJump, BTICKSPERFRAME, -1, 0.1f, 0, NULL, NULL);

		this->setState(ZBBALLSTATE_SKIDDING);
	}
	else// PP: GOING SLOW
	{
		// PP: play scared jump noise
		this->playSound("elescaredjump.wav", AIUSOUND_TRACK);

		// PP: JUST JUMP WHILE TURNING TO FACE THE OTHER WAY
		baQueueNodeAnimBlendState(&(this->actorInstance->actorInstance), this->actorInstance->actorInstance.rootNodeInstance, this->anims.jump, BTICKSPERFRAME, -1, 0.1f, 0, NULL, NULL);

		this->duration=ANIMTIME(this->anims.jump);
		this->setState(ZBBALLSTATE_JUMPING); 
	}

	// PP: store angle before turn
	this->srcYang=this->getYang();

	// PP: set the angle that the ball's going to turn to
	float			ang;
	TBVector2D		dist;

	// PP: get ball-bat distance...

	if(bat.actorInstance->actorStatus->currentState == STATE_SPIN)// PP: bat is super-spinning into ball
	{
		// PP: use ball's centre point cos the bat will be so close
		dist[X]=this->pos()[X]-bat.pos()[X];
		dist[Y]=this->pos()[Z]-bat.pos()[Z];
	}
	else// PP: bat is scaring ball
	{
		// PP: use ball's head point cos the angles look better that way
		dist[X]=this->headPos[X]-bat.pos()[X];
		dist[Y]=this->headPos[Z]-bat.pos()[Z];
	}

	ang=bmATan2(dist[Y], dist[X]);

	ang+=HALFPI;

	// PP: TEST: EXAGGERATE THE ANGLE DIFFERENCE...

//#define ZBBALL_DEFLECTION_ANGDIFF_FACTOR		1.0f

	float	angDiff;
	
	angDiff=FindShortestAngleSigned(ang, bat.getNormalYang());
//	angDiff*=ZBBALL_DEFLECTION_ANGDIFF_FACTOR;

	if(angDiff > 0.0f)
	{
		angDiff=MIN(ZBBALL_DEFLECTION_MAX_ANGDIFF, angDiff);
	}
	else if (angDiff < 0.0f)
	{
		angDiff=MAX(-ZBBALL_DEFLECTION_MAX_ANGDIFF, angDiff);
	}

	ang=Aabs(bat.getNormalYang()+angDiff);

	this->destYang=Aabs(ang);

	// PP: TEST cancel any turn in progress
	this->flags &= ~ZBBALLFLAG_TURNING;

	// PP: clear wall contact latches to stop nasty things happening when we finish the skid & jump
	this->TBwallContactLatch=false;
	this->RLwallContactLatch=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::innerCollide
	Purpose 	: check for inner collision with a bat (the type that'd squash you if you weren't scaring)
	Parameters 	: bat with which to check for collision
	Returns 	: 
	Info 		: 
*/
bool ZBBALL::innerCollide(const ZBBAT& bat) const
{
	// PP: I'm going to be very simplistic about the collision till I have good reason not to be!
	float dist=bmVectorDistance(bat.pos(), this->pos());
	float maxRadius=MAX(this->innerCollRadius, bat.radius);

	return (dist<maxRadius);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::outerCollide
	Purpose 	: check for outer collision with a bat (the type that'd knock you back while you're scaring)
	Parameters 	: bat with which to check for collision
	Returns 	: 
	Info 		: 
*/
bool ZBBALL::outerCollide(const ZBBAT& bat) const
{
	// PP: I'm going to be very simplistic about the collision till I have good reason not to be!
	float dist=bmVectorDistance(bat.pos(), this->pos());
	float maxRadius=MAX(this->outerCollRadius, bat.radius);
	return (dist<maxRadius);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::tuskCollide
	Purpose 	: check for tusk collision with a bat (the type that'd prod you)
	Parameters 	: bat with which to check for collision
	Returns 	: 
	Info 		: 
*/
bool ZBBALL::tuskCollide(const ZBBAT& bat) const
{
	// PP: I'm going to be very simplistic about the collision till I have good reason not to be!
	float dist=bmVectorDistance(bat.pos(), this->pos());
	float maxRadius=MAX(this->tuskCollRadius, bat.radius);

	if(dist>maxRadius) return FALSE;

	return (this->getYAngFromLOS(bat) < ZBBALL_TUSK_ANGLE_RANGE);
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::hear
	Purpose 	: check if the ball can hear a bat (assuming the bat is scaring)
	Parameters 	: bat with which to check for hearing
	Returns 	: 
	Info 		: 
*/
bool ZBBALL::hear(const ZBBAT& bat)
{
	// PP: ball must be in the right state to hear the bat
	if(this->state != ZBBALLSTATE_RUNNING)
	{
		return false;
	}

	if(bat.dist(this->headPos) > (bat.noiseLevel*ZBBALL_HEARING_RADIUS))
	{
		return false;// PP: out of range
	}

	// PP: Dirty you may think, but this saves time - as a test at least
	if(bat.type == ZBBATTYPE_TAZ)
	{
		if(this->headPos[Z]>bat.pos()[X])
		{
			return false;// PP: behind bat
		}
	}
	else// PP: gossamer
	{
		if(this->headPos[Z]<bat.pos()[X])
		{
			return false;// PP: behind bat
		}
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::applyNormalMovement
	Purpose 	: move the ball in the way that it moves while running or rebounding from a wall
	Parameters 	: angle of movement
	Returns 	: 
	Info 		: 
*/
void ZBBALL::applyNormalMovement(const float yAng)
{
	float		moveDist;
	TBVector	motionVec;


	// PP: calculate speed
	this->speed=ZBBALL_MIN_MOVE_DIST+(this->speedFraction*(ZBBALL_MAX_MOVE_DIST-ZBBALL_MIN_MOVE_DIST));

	// PP: get distance to move this frame
	moveDist=this->speed/fps;

	// PP: apply run movement in direction of ball
	SETVECTOR(motionVec, moveDist*bmSin(TWOPI-yAng+PI), 0.0f, moveDist*bmCos(TWOPI-yAng+PI), 1.0f);
	bmVectorAdd(this->actorInstance->actorInstance.position, this->actorInstance->actorInstance.position, motionVec);
}



/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::testWallCollsions
	Purpose 	: check for, and respond to, collisions of the ball with the court walls
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::testWallCollisions(void)
{
	float			wallAng;
	const float*	wallPtr;
	bool			newContact=false;
	bool			protectedWall=false;


	if(this->pos()[Z] > this->contactCourt.top)// PP: if we're in contact with the top of the court
	{
		if(!TBwallContactLatch)// PP: only proceed if the contact is new
		{
			// PP: only proceed if the ball is facing the wall - we don't want it to turn straight back into it
			if(FindShortestAngle(this->getYang(), PI) < HALFPI)
			{
				wallAng=PI+HALFPI;
				wallPtr= &this->court.top;

				newContact=true;

				TBwallContactLatch=true;

				bkPrintf("TOP\n");
			}
		}
	}
	else if(this->pos()[Z] < this->contactCourt.bottom)// PP: if we're in contact with the bottom of the court
	{
		if(!TBwallContactLatch)// PP: only proceed if the contact is new
		{
			if(FindShortestAngle(this->getYang(), 0.0f) < HALFPI)
			{
				wallAng=HALFPI;
				wallPtr= &this->court.bottom;

				newContact=true;

				TBwallContactLatch=true;

				bkPrintf("BOTTOM\n");
			}
		}
	}
	else
	{
		TBwallContactLatch=false;
	}

	if(this->pos()[X] > this->contactCourt.right)// PP: if we're in contact with the right of the court
	{
		if(!RLwallContactLatch)// PP: only proceed if the contact is new
		{
			// PP: only proceed if the ball is facing the wall - we don't want it to turn straight back into it
			if(FindShortestAngle(this->getYang(), HALFPI) < HALFPI)
			{
				protectedWall=true;

				wallAng=PI;

				wallPtr= &this->court.right;

				newContact=true;

				RLwallContactLatch=true;

				bkPrintf("RIGHT\n");
			}
		}
	}
	else if(this->pos()[X] < this->contactCourt.left)// PP: if we're in contact with the left of the court
	{
		if(!RLwallContactLatch)// PP: only proceed if the contact is new
		{
			// PP: only proceed if the ball is facing the wall - we don't want it to turn straight back into it
			if(FindShortestAngle(this->getYang(), PI+HALFPI) < HALFPI)
			{
				protectedWall=true;

				wallAng=0.0f;

				wallPtr= &this->court.left;

				newContact=true;

				RLwallContactLatch=true;

				bkPrintf("LEFT\n");
			}
		}
	}
	else
	{
		RLwallContactLatch=false;
	}


//	secondContact=this->RLwallContactLatch&&this->TBwallContactLatch;

	if(newContact)
	{
		if(protectedWall)
		{
			this->koikWall=wallPtr;

			this->setState(ZBBALLSTATE_GOAL);
		}
		else
		{
		/*	if(secondContact)
			{
				// PP: this is a secondary contact - just reverse the angle of the ball before its first contact
				this->destYang=Aabs(this->srcYang+PI);
			}
			else*/
			{
				// PP: this is a primary collision: set a normal deflection angle for the ball

				this->destYang=Aabs(wallAng+(wallAng-this->getYang()));

				this->srcYang=this->getYang();
			}

			// PP: TEST: make a boing noise
			this->playSound("elebounce.wav", AIUSOUND_POSITION);

			this->flags |= ZBBALLFLAG_TURNING;
		}
	}

/*	else
	{
		// PP: moveme?
		if(!(this->flags & ZBBALLFLAG_TURNING))
		{
			this->destYang=this->getYang();
		}
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::footstep
	Purpose 	: elephant footstep, bleh, i'm tired
	Parameters 	: ptr to ACTORINSTANCE doing the footstep (could be cutscene elephant),
					which foot/feet (ZBBALLFOOT_), (opt/false)is this footstep part of a ZBBALLFOOT_ALL
	Returns 	: 
	Info 		: 
*/

#define ZBBALL_FOOTSTEP_STRENGTH_WALKING		1.0f			// PP: let's use this as the yardstick for all camera shakes
#define ZBBALL_FOOTSTEP_STRENGTH_RUNNING		1.3f
#define ZBBALL_FOOTSTEP_STRENGTH_ALL			2.0f
#define ZBBALL_FOOTSTEP_STRENGTH_DEVIATION		0.15f

// PP: footprint stuff...

#define ZBBALL_FOOTPRINT_SIZE					110.0f
#define ZBBALL_FOOTPRINT_LIFE					7.0f

#define ZBBALL_FOOTPRINT_SUB_COLOUR				COLOUR(60, 50, 35, 128)
//#define ZBBALL_FOOTPRINT_ADD_COLOUR			COLOUR(15, 13, 8, 255)

void ZBBALL::footstep(const ACTORINSTANCE* const footstepActorInstance, const int foot, const bool all)
{
	if(foot == ZBBALLFOOT_ALL)
	{
		// PP: NOTE ***** DURING THE INTRO CUTSCENE, AN ALL-HOOVES FOOTSTEP MESSAGE IS SENT FROM THE CUTSCENE_ELEPHANT *****
		// PP: WHEN THIS HAPPENS, I GENERATE THE DUST BUT DON'T DO ANYTHING ELSE...

#ifdef ZB_PONGDUST
		PongDust_ring(footstepActorInstance->actorInstance.position);
#endif// PP: pongdust

		if(footstepActorInstance != this->actorInstance)
		{
			// PP: cutscene elephant
			return;
		}

		for(int f=0; f<4; f++)
		{
			this->footstep(this->actorInstance, f, true);
		}

		// PP: SHAKE THE CAMERA
		ZooBoss_game.groundPoundCameraShake(this->pos(),
											ZBBALL_FOOTSTEP_STRENGTH_ALL,
											ZBBALL_FOOTSTEP_STRENGTH_DEVIATION);

		// PP: MAKE STOMP NOISE
		this->playSound("elefstep.wav", AIUSOUND_POSITION);

		return;
	}
	else
	{
		// PP: ONLY ALL_HOOVES FOOTSTEP MESSAGES ARE HANDLED FOR THE CUTSCENE ELEPHANT
		ASSERT(footstepActorInstance == this->actorInstance);
	}

	baGetNodesWorldPosition(&this->actorInstance->actorInstance, this->steps[foot].node, this->steps[foot].pos);

	/* PP: TODO: ******************************** listen up

	vibration

	// **********************/

	// PP: MAKE A FOOTPRINT...

	TBVector	footprintPos;

	bmVectorCopy(footprintPos, this->steps[foot].pos);
	footprintPos[Y]=ZB_COURT_HEIGHT;

	CreateFloorDecal(ZBBALL::footprintTexture, footprintPos, this->getYang(), ZBBALL_FOOTPRINT_SIZE, ZBBALL_FOOTPRINT_LIFE, ZBBALL_FOOTPRINT_SUB_COLOUR, DECALBLEND_IMPRESS/*DECALBLEND_BLEND*/, /*ZBBALL_FOOTPRINT_ADD_COLOUR*/ZBBALL_FOOTPRINT_SUB_COLOUR, ZooBoss_sunDir.v);

	//PlaySample("explo_gen_01b.wav", 255);

	//bkPrintf("play sample, %d %5.2f\n", f, this->runPhase);

	if(!all)
	{
		// PP: SHAKE THE CAMERA
		ZooBoss_game.groundPoundCameraShake(footprintPos,
											ZBBALL_FOOTSTEP_STRENGTH_WALKING+(this->speedFraction*(ZBBALL_FOOTSTEP_STRENGTH_RUNNING-ZBBALL_FOOTSTEP_STRENGTH_WALKING)),
											ZBBALL_FOOTSTEP_STRENGTH_DEVIATION);

		// PP: MAKE STOMP NOISE
		// PP: hind legs make different stomp noise from front legs -
		// PP: that's a little-known FACT about elephants
		if(foot&1)
		{
			this->playSound("elefstep.wav", AIUSOUND_POSITION);
		}
		else
		{
			this->playSound("elefstep1.wav", AIUSOUND_POSITION);
		}

		{
			static uint32	amplitude=200;
			static float	duration=0.1f;
			static float	attack=0.05f;
			static float	sustain=0.4f;
			static float	range=METERTOUNIT(8);

			RequestRumbleEffect(&controller1, /* PP: REMOUT:  bmFloatToInt(amplitude*(1.0f-(this->XZdist(ZooBoss_game.bats[0])/range)))*/ amplitude, attack, sustain, duration, 0.0f/*delay*/, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);

			if(ZooBoss_game.twoPlayer)
			{
				RequestRumbleEffect(&controller2, /* PP: REMOUT:  bmFloatToInt(amplitude*(1.0f-(this->XZdist(ZooBoss_game.bats[1])/range)))*/amplitude, attack, sustain, duration, 0.0f/*delay*/, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::startMove
	Purpose 	: set the ball moving
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZBBALL::startMove(void)
{
	this->actorInstance->actorAnimation->useMotionBone = FALSE;
}


/*	--------------------------------------------------------------------------------
	Function 	: ZBBALL::controlEyes
	Purpose 	: control the ball's eyes
	Parameters 	: eye action to perform (ZBBALLEYES_)
	Returns 	: 
	Info 		: 
*/
void ZBBALL::controlEyes(const ZBBallEyeAction action)
{
	switch(action)
	{
	case ZBBALLEYES_OPEN:
		{
			bdSetTextureFrame(this->eyeTexture, 0);// PP: open
		}
		break;

	case ZBBALLEYES_CLOSE:
		{
			bdSetTextureFrame(this->eyeTexture, 1);// PP: closed

			// PP: play blink noise as eyes close
			PlaySample("eyesblink1.wav", 255);// PP: NOTE: I had to put this into the cutscene sounds to get it to play (?)
		}
		break;

	case ZBBALLEYES_BLINK:
		{
			bdSetTextureFrame(this->eyeTexture, 1);// PP: closed

			this->blinkTimer=ZBBALL_BLINK_DURATION;
		}
		break;

		NODEFAULT;
	}
}


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*	--------------------------------------------------------------------------------
	Function 	: ZooBoss_createCutsceneSpinDust
	Purpose 	: create pong dust for taz's spin in the cutscene
	Parameters 	: 
	Returns 	: true if pong dust was appropriate, else false (use normal spin dust)
	Info 		: // PP: only call this on Zoo Boss cutscene
*/
bool ZooBoss_createCutsceneSpinDust(const VEC& pos)
{
	// PP: TODO?: keep a record of the position from when this was last called.
	// PP: use this to interpolate cloud positions so's the dust never looks bitty.

#ifdef ZB_PONGDUST

	// PP: REMOUT: but it's so cuuute!		if(ZooBoss_game.court.contains(pos[X], pos[Z]))
	{
		PongDust_create(pos, 1, ZVEC, ZBBAT_SPIN_DUST_FREQUENCY);

		return true;
	}
	/*else
	{
		return false;
	}*/

#else// PP: no pongdust

	return false;// PP: no can do

#endif// PP: no pongdust
}


/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::prepareForPause
	Purpose 	: make preparations for entering pause mode while in the Zoo Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ZOOBOSSGAME::prepareForPause(void)
{
	// PP: close books that would clash with the pause menus
	// PP: NOTE: this would have been handled by the 'Exclusive Book' stuff
	// PP: if I'd had a chance to finish it / test it.
	this->messageBook.close();// PP: goal / you win / you lose etc.

	for(int b=0; b<BOSS_NUM_PLAYERS; b++)
	{
		this->bats[b].book.close();// PP: scare!
	}
}