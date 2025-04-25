// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Cheats.h
//   Purpose : Cheats
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_CHEATS
#define __H_CHEATS


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HOW TO ADD A CHEAT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


1.	Add a new entry for your cheat into the 'ECheat' enumeration in Cheats.h.

2.	Add the name of your cheat into Main.xls, amongst the other 'CHEAT_' strings, at the position corresponding to that of your
	new ECheat entry.

3.	If you want something to happen when your cheat is ACTIVATED, add that code into the appropriate position in the switch
	statement in ActivateCheat (Cheats.cpp).

4.	If you want something to happen when your cheat is DEACTIVATED, add that code into the appropriate position in the switch
	statement in DeactivateCheat (Cheats.cpp).

5.	If you want something to happen if your cheat is ACTIVE, check using CheatActive (Cheats.cpp), passing in your new ECheat index.



									// PP: Please don't hesitate to ask, if anything is unclear.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															CHEATS

					// PP: I thought I'd better set this up now, since cheats can be useful for debugging.
		Also, having a cheat system means that redundant-but-cool features needn't be lost or left commented out untidily - 
												we can just turn them into cheats.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: define INGAME_CHEATS_MENU to enable the in-game cheats menu, accessible via the pause menu
#ifndef INFOGRAMES
#endif// PP: ndef INFOGRAMES

// PP: enumeration of all cheats
enum ECheat
{
	CHEAT_NONE,									// PP: no cheats - please leave this in!
		
// PP: **********************************************
// PP: **********************************************

	CHEAT_OPEN_GALLERY,							// NH: Allows player to enter all gallerys
	CHEAT_OPEN_BOSS_GAME,						// NH: Allows player to enter all boss games
	CHEAT_OPEN_BONUS_GAMES,						// NH: Allows player to enter all bonus games
	CHEAT_DIGITALWATCH,							// PP: Taz wears his digital watch that tells the system time
	CHEAT_TURBO,								// PP: everything is fast
	CHEAT_SLOWMO,								// PP: everything is slow
	CHEAT_SUPERSLOWMO,							// PP: everything is very slow
	CHEAT_INVISIBLE,							// PP: ain't nobody can see you man
	CHEAT_INVINCIBLE,							// PP: you no get hurt no more
	CHEAT_COLOURSTEREOSCOPIC,					// PP: colour stereoscopic mode (faster but with brightness discrepancies)
	CHEAT_GREYSTEREOSCOPIC,						// PP: grey stereoscopic mode (slower but with no brightness discrepancies)
	CHEAT_QWILSON,								// TP: Pictures of wilson appear in interesting places
	CHEAT_DISABLESECURITYBOXES,					// PP: disables all security boxes so's you can get around a bit easier
	CHEAT_MOTIONBLUR,							// PP: motion blur, or 'gin mode'
	CHEAT_COSTUME,								// PP: wear costume
	CHEAT_TIMESLICENINJAKICK,					// PP: go all Matrix when doing the ninja kick special move
	CHEAT_TIMESLICEJUMP,						// PP: go all Matrix when jumping								
	CHEAT_INSOMNIA,								// PP: Taz doesn't sleep.  See how HE likes it.
	CHEAT_OPENLEVELS,							// TP: Allows the player to enter and exit any levels complete or not

#ifdef HENRIK

	NUM_CHEATS,									// PP: number of cheats for Henrik

#define CHEAT_DUMMY					=INTMAX,	// PP: dummy cheat with very high index - pushes the cheats after it out of range

#endif// PP: def HENRIK

#define FIRST_DEBUG_CHEAT						CHEAT_SHADOWMAP		// PP: PS2-friendly blurred character shadow maps

	FIRST_DEBUG_CHEAT,				// PP: ie. CHEAT_SHADOWMAP

// PP: **************************************************
// PP: **************************************************

	CHEAT_AIRBRUSH,								// PP: puts smooth shading on the characters, for an airbrushed effect
	CHEAT_NEARFOCUS,							// PP: focus on near objects and blur distant ones
	CHEAT_FARFOCUS,								// PP: focus on distant objects and blur near ones
	CHEAT_DEFOCUS,								// PP: focus on nothing, blur the whole scene		
	CHEAT_FILTERING,							// CPW: muck about with the filtering
	CHEAT_TIMEFREEZE,							// PP: Time freezes completely, but the flycam can still be used
	CHEAT_GLOVER_MODE,							// PP: Taz turns into a Glover ball
	CHEAT_SHOWDEBUGINFO,						// CMD: use this to toggle drwing of debug info
	CHEAT_TAZCAM,								// PP: Attaches the camera to Taz's heed

#ifndef HENRIK

	NUM_CHEATS									// PP: number of cheats

#endif// PP: def HENRIK
};

// PP: get the index for the text string naming the cheat at _index
#define CHEAT_NAME(_index)					((EStringIndex)(STR_CHEATNAME_NONE+(_index)))

// PP: maximum length of a cheat name string
#define CHEATS_MAX_NAME_LENGTH				256
															
#define NO_CHEATS							0						// PP: no cheats
#define ALL_CHEATS							((1<<NUM_CHEATS)-1)		// PP: combination of all cheats
															

#define CHEATS_TURBO_SPEED_FACTOR			2.0f	// PP: game speed factor of TURBO cheat
#define CHEATS_SLOWMO_SPEED_FACTOR			0.5f	// PP: game speed factor of SLOWMO cheat
#define CHEATS_SUPERSLOWMO_SPEED_FACTOR		0.25f	// PP: game speed factor of SUPERSLOWMO cheat
#define CHEAT_TIMESLICENINJAKICK_DELAY		0.4f	// PP: time into the kick anim before the timeslice is triggered, for CHEAT_TIMESLICENINJAKICK

typedef struct TAG_CHEATSSTRUCT
{
	float				cheatsTextScale;

	struct TAG_TEXTBOX	*cheatsTextTb;
	struct TAG_PAGE		*cheatsPage;
	struct TAG_BOOK		*cheatsBook;
	bool				cheatTextShowing;

}CHEATSSTRUCT, _CHEATSSTRUCT BALIGN16;

extern CHEATSSTRUCT	cheatsStruct;
//CHEATSPAGE				*cheatsPage;

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseCheats
	Purpose 	: initialise stuff for cheats
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void InitialiseCheats(void);


/*	--------------------------------------------------------------------------------
	Function 	: UpdateCheats
	Purpose 	: per-frame update of cheats
	Parameters 	: 
	Returns 	: 
	Info 		: may be temp
*/
void UpdateCheats(void);


/*	--------------------------------------------------------------------------------
	Function 	: AwardCheat
	Purpose 	: award a cheat, make it available to the player
	Parameters 	: cheat to award
	Returns 	: 
	Info 		: 
*/
void AwardCheat(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Macro	 	: CheatActive
	Purpose 	: determine if a particular cheat is active
	Parameters	: ECheat index of cheat to check for
	Value		: non-zero (true) if the cheat is active, else zero (false)
	Info 		: // PP: macro'd this to prevent hoy with 'gameStatus'
*/
#define CheatActive(_cheat)		(gameStatus.globalSettings.cheatsActive & (1<<((_cheat)-1)))


/*	--------------------------------------------------------------------------------
	Function 	: CheatAvailable
	Purpose 	: determine if a particular cheat is available to the player
	Parameters 	: cheat to check for
	Returns 	: true if the cheat is available to the player, false if not
	Info 		: 
*/
bool CheatAvailable(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Function 	: ActivateCheat
	Purpose 	: activate a cheat, switch it on
	Parameters 	: index of cheat to activate
	Returns 	: 
	Info 		: 
*/
void ActivateCheat(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Function 	: DeactivateCheat
	Purpose 	: deactivate a cheat, switch it off
	Parameters 	: index of cheat to deactivate
	Returns 	: true if the deactivation succeeded, false if the cheat wasn't on in the first place
	Info 		: *** THIS ASSUMES THAT THE APPROPRIATE CHECKS HAVE ALREADY BEEN DONE TO PROTECT CHEAT EFFECTS ***
*/
bool DeactivateCheat(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Function 	: ToggleCheat
	Purpose 	: toggle a cheat on/off
	Parameters 	: ECheat to toggle, (opt/false) give the player feedback to confirm that the action has been taken
	Returns 	: 
	Info 		: 
*/
void ToggleCheat(const ECheat cheat, const bool playerNotification=false);


/*	--------------------------------------------------------------------------------
	Function 	: DeactivateAllCheats
	Purpose 	: deactivate all cheats, switch them all off
	Parameters 	: (opt/false) give the player feedback to confirm that the action has been taken
	Returns 	: 
	Info 		: All Special dishes are served with Pilau Rice or Naan
*/
void DeactivateAllCheats(const bool playerNotification=false);


/*	--------------------------------------------------------------------------------
	Function 	: CheatImplemented
	Purpose 	: find out if a cheat is earnable by the player
	Parameters 	: cheat to check for
	Returns 	: 
	Info 		: 
*/
bool CheatImplemented(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Function 	: guiCheatImplemented
	Purpose 	: find out if a cheat is available from the gui
	Parameters 	: cheat to check for
	Returns 	: 
	Info 		: 
*/
bool guiCheatImplemented(const ECheat cheat);


/*	--------------------------------------------------------------------------------
	Function 	: checkCheats
	Purpose 	: checks to see if a cheat was entered in as a player name
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void checkCheats(struct TAG_PLAYERSTATS *stats);

/*	--------------------------------------------------------------------------------
	Function 	: ShutdownCheats
	Purpose 	: shutdown stuff for cheats
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ShutdownCheats(void);

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_DIGITALWATCH

									Taz wears his digital watch that tells the system time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: is the player trying to look at the watch? the little monkey?  This is going to be so cool.
extern bool					watchLook;


/*	--------------------------------------------------------------------------------
	Function 	: LookAtWatch
	Purpose 	: point the camera at the watch, close enough that you can read the time off it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void LookAtWatch(void);


/*	--------------------------------------------------------------------------------
	Function 	: CheckForWatchLook
	Purpose 	: look at pad input to determine whether or not the player is
					trying to look at the watch
	Parameters 	: 
	Returns 	: 
	Info 		: This function sets 'watchLook'.
					check before calling that CHEAT_DIGITALWATCH is active
*/
void CheckForWatchLook(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_QWILSON

									Pictures of wilson appear in interesting places.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: ptr to Qwilson's pic
extern TBTexture* QwilsonTexture;

// PP: and to the back of the goth mask
extern TBTexture* QwilsonBackTexture;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_TIMEFREEZE

									Time freezes completely, but the flycam can still be used.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: timeFreezePauseUpdateFunc
	Purpose 	: pause update function for the time-freeze cheat
	Parameters 	: parameter is not used
	Returns 	: 
	Info 		: updates fly cam
*/
void timeFreezePauseUpdateFunc(const int32 parameter);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
										stuff for CHEAT_NEARFOCUS & CHEAT_FARFOCUS

										Focus effects, still undergoing experimentation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


extern float g_testDOFNear;
extern float g_testDOFFar;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_TAZCAM

											Attaches the camera to Taz's heed.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "camera.h"								// PP: camera functions

extern CAMERAINFO			tazCam;				// PP: the camera attached to Taz's heed


/*	--------------------------------------------------------------------------------
	Function 	: updateTazCam
	Purpose 	: update the position and orientation of the Taz Cam
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: gets called if CheatActive(CHEAT_TAZCAM)
*/
void updateTazCam(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												NAUGHTY STUFF (teehee!)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum EWallpaperMode
{
	WALLPAPER_CENTRE,
	WALLPAPER_TILE,
	WALLPAPER_STRETCH
};


/*	--------------------------------------------------------------------------------
	Function 	: SetWallpaper
	Purpose 	: Set the specified user's wallpaper
	Parameters 	: user name string eg "ahodgson", wallpaper file eg "\\\\ppalmer\\c\\box_01.bmp", (opt/WALLPAPER_STRETCH)wallpaper mode (_STRETCH/_TILE/_CENTRE)
	Returns 	: 
	Info 		: this is probably set up for Win 2000, so check those paths first
*/
void SetWallpaper(const char* const userName, const char* const wallpaper, const EWallpaperMode mode=WALLPAPER_STRETCH);


#endif // PP: ndef __H_CHEATS